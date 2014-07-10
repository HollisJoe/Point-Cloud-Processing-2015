#include "WLOP.h"


WLOP::WLOP(RichParameterSet* _para)
{
	cout << "WLP constructed!!" << endl;
	para = _para;
	samples = NULL;
	original = NULL;
	nTimeIterated = 0;
	error_x = 0.0;
}

WLOP::~WLOP(void)
{
	cout << "WLop destroy!! " << endl; 
}

void WLOP::clear()
{
	samples = NULL;
	original = NULL;
}

void WLOP::setFirstIterate()
{
	nTimeIterated = 0;
}

void WLOP::setInput(DataMgr* pData)
{
	if(!pData->isSamplesEmpty() && !pData->isOriginalEmpty())
	{
		CMesh* _samples = pData->getCurrentSamples();
		CMesh* _original = pData->getCurrentOriginal();

    if (para->getBool("Run Skel WLOP"))
    {
      _original = pData->getCurrentDualSamples();
    }

		if(_samples == NULL || _original == NULL)
		{
			cout << "ERROR: WLOP::setInput == NULL!!" << endl;
			return;
		}

		error_x = 0.0;
		samples = _samples;
		original = _original;
    dual_samples = pData->getCurrentDualSamples();

    if (para->getBool("Run Dual WLOP"))
    {
      samples = pData->getCurrentDualSamples();
    }
		samples_density.assign(samples->vn, 1);
	}
	else
	{
		cout << "ERROR: WLOP::setInput: empty!!" << endl;
		return;
	}
}


void WLOP::initVertexes()
{
  bool use_current_neighbor = para->getBool("Use Adaptive Sample Neighbor");
	
  box.SetNull();
	CMesh::VertexIterator vi, vi_end;

	int i = 0;
	vi_end = samples->vert.end();
	for(vi = samples->vert.begin(); vi != vi_end; ++vi) 
	{
		vi->m_index = i++;
    if (!use_current_neighbor)
    {
      vi->neighbors.clear();
    }
		vi->original_neighbors.clear();

		if (vi->is_skel_ignore)
		{
			continue;
		}
		box.Add(vi->P());
	}
	samples->bbox = box;


	vi_end = original->vert.end();
	i = 0;
	for(vi = original->vert.begin(); vi != vi_end; ++vi) 
	{
		vi->m_index = i++;
		box.Add(vi->P());
	}
	original->bbox = box;


	repulsion.assign(samples->vn, vcg::Point3f(0, 0, 0));
	average.assign(samples->vn, vcg::Point3f(0, 0, 0));

	repulsion_weight_sum.assign(samples->vn, 0);
	average_weight_sum.assign(samples->vn, 0);

  samples_average.assign(samples->vn, vcg::Point3f(0, 0, 0));
  samples_average_weight_sum.assign(samples->vn, 0);

	if (para->getBool("Need Compute PCA"))
	{
		CVertex v;
		mesh_temp.assign(samples->vn, v);
	}
}


void WLOP::run()
{
	if (para->getBool("Run Anisotropic LOP"))
	{
		cout << "Run Anisotropic LOP" << endl;
	}

  if (para->getBool("Run Step Forward"))
  {
    cout << "Run Step Forward" << endl;
    stepForward();
    return;
  }

  if (para->getBool("Run Compute Initial Sample Neighbor"))
  {
    computeInitialSampleNeighbor();
    return;
  }

  if (para->getBool("Run Skel WLOP"))
  {
    runSkelWlop();
    return;
  }

  if (para->getBool("Run Dual Drag WLOP"))
  {
    computeDiskNeighborhood();
    //runDragWlop();
    return;
  }

  if (para->getBool("Run Regularize Samples"))
  {
    runRegularizeSamples();
    return;
  }

	//int nTimes = para->getDouble("Num Of Iterate Time");
	for(int i = 0; i < 1; i++)
	{ 
		iterate();
		
		nTimeIterated ++;
		cout << "Iterated: " << nTimeIterated << endl;
	}
	cout << "**************iterate Number: " << nTimeIterated << endl;
}

void WLOP::computeAverageAddSampleTerm(CMesh* samples, CMesh* original)
{
  double average_power = para->getDouble("Average Power");
  bool need_density = para->getBool("Need Compute Density");
  double radius = para->getDouble("CGrid Radius"); 

  double radius2 = radius * radius;
  double iradius16 = -para->getDouble("H Gaussian Para")/radius2;

  for(int i = 0; i < samples->vert.size(); i++)
  {
    CVertex& v = samples->vert[i];

    for (int j = 0; j < v.original_neighbors.size(); j++)
    {
      CVertex& t = original->vert[v.original_neighbors[j]];

      Point3f diff = v.P() - t.P();
      double dist2  = diff.SquaredNorm();

      double w = 1;
      if (para->getBool("Run Anisotropic LOP"))
      {
        double len = sqrt(dist2);
        if(len <= 0.001 * radius) len = radius*0.001;
        double hn = diff * v.N();
        double phi = exp(hn * hn * iradius16);
        w = phi / pow(len, 2 - average_power);
      }
      else if (average_power < 2)
      {
        double len = sqrt(dist2);
        if(len <= 0.001 * radius) len = radius*0.001;
        w = exp(dist2 * iradius16) / pow(len, 2 - average_power);
      }
      else
      {
        w = exp(dist2 * iradius16);
      }

      if (need_density)
      {
        w *= original_density[t.m_index];
      }

      average[i] += t.P() * w;  
      average_weight_sum[i] += w;  
    }

    for (int k = 0; k < v.neighbors.size(); k++)
    {
      CVertex& t = samples->vert[v.neighbors[k]];

      Point3f diff = v.P() - t.P();
      double dist2  = diff.SquaredNorm();

      double w = 1;
      if (para->getBool("Run Anisotropic LOP"))
      {
        double len = sqrt(dist2);
        if(len <= 0.001 * radius) len = radius*0.001;
        double hn = diff * v.N();
        double phi = exp(hn * hn * iradius16);
        w = phi / pow(len, 2 - average_power);
      }
      else if (average_power < 2)
      {
        double len = sqrt(dist2);
        if(len <= 0.001 * radius) len = radius*0.001;
        w = exp(dist2 * iradius16) / pow(len, 2 - average_power);
      }
      else
      {
        w = exp(dist2 * iradius16);
      }

      if (need_density)
      {
        w *= 1.0 / (samples_density[t.m_index] * samples_density[t.m_index]);
      }

      average[i] += t.P() * w;  
      average_weight_sum[i] += w;  
    }
  }
}

void WLOP::computeAverageTerm(CMesh* samples, CMesh* original)
{
	double average_power = para->getDouble("Average Power");
	bool need_density = para->getBool("Need Compute Density");
  bool use_elliptical_neighbor = para->getBool("Use Elliptical Original Neighbor");
	double radius = para->getDouble("CGrid Radius"); 

	double radius2 = radius * radius;
	double iradius16 = -para->getDouble("H Gaussian Para")/radius2;
  
  double close_threshold = radius2 / 16;
  
	cout << "Original Size:" << samples->vert[0].original_neighbors.size() << endl;
  for(int i = 0; i < samples->vert.size(); i++)
	{
		CVertex& v = samples->vert[i];

		for (int j = 0; j < v.original_neighbors.size(); j++)
		{
			CVertex& t = original->vert[v.original_neighbors[j]];
			
			Point3f diff = t.P() - v.P();
      double proj_dist = diff * v.N();
      double proj_dist2 = proj_dist * proj_dist;

			double dist2  = diff.SquaredNorm();

			double w = 1;
			if (para->getBool("Run Anisotropic LOP"))
			{
				double len = sqrt(dist2);
				if(len <= 0.001 * radius) len = radius*0.001;
				double hn = diff * v.N();
				double phi = exp(hn * hn * iradius16);
				w = phi / pow(len, 2 - average_power);
			}
			else if (average_power < 2)
			{
				double len = sqrt(dist2);
				if(len <= 0.001 * radius) len = radius*0.001;
				w = exp(dist2 * iradius16) / pow(len, 2 - average_power);
			}
			else
			{
				w = exp(dist2 * iradius16);
			}

      if (use_elliptical_neighbor)
      {
        w *= exp(proj_dist2 * iradius16);
      }

			if (need_density)
			{
				w *= original_density[t.m_index];
			}

			average[i] += t.P() * w;  
			average_weight_sum[i] += w;  


      //if (use_adaptive_mu && !is_sample_close_to_original[v.m_index])
      //{
      //  if (dist2 < close_threshold)
      //  {
      //    is_sample_close_to_original[v.m_index] = true;
      //  }
      //}
		}
	}
}


void WLOP::computeRepulsionTerm(CMesh* samples)
{
	double repulsion_power = para->getDouble("Repulsion Power");
	bool need_density = para->getBool("Need Compute Density");
	double radius = para->getDouble("CGrid Radius"); 

	double radius2 = radius * radius;
	double iradius16 = -para->getDouble("H Gaussian Para")/radius2;

	cout << endl<< endl<< "Sample Neighbor Size:" << samples->vert[0].neighbors.size() << endl<< endl;
	for(int i = 0; i < samples->vert.size(); i++)
	{
		CVertex& v = samples->vert[i];
		for (int j = 0; j < v.neighbors.size(); j++)
		{
			CVertex& t = samples->vert[v.neighbors[j]];
			Point3f diff = v.P() - t.P();

			double dist2  = diff.SquaredNorm();
			double len = sqrt(dist2);
			if(len <= 0.001 * radius) len = radius*0.001;

			double w = exp(dist2*iradius16);
			double rep = w * pow(1.0 / len, repulsion_power);

			if (need_density)
			{
				rep *= samples_density[t.m_index];
			}

			repulsion[i] += diff * rep;  
			repulsion_weight_sum[i] += rep;
		}
	}
}

void WLOP::computeSampleAverageTerm(CMesh* samples)
{
  double repulsion_power = para->getDouble("Repulsion Power");
  bool need_density = para->getBool("Need Compute Density");
  double radius = para->getDouble("CGrid Radius"); 

  double radius2 = radius * radius;
  double iradius16 = -para->getDouble("H Gaussian Para")/radius2;

  cout << endl<< endl<< "Sample Neighbor Size:" << samples->vert[0].neighbors.size() << endl<< endl;
  for(int i = 0; i < samples->vert.size(); i++)
  {
    CVertex& v = samples->vert[i];
    for (int j = 0; j < v.neighbors.size(); j++)
    {
      CVertex& t = samples->vert[v.neighbors[j]];
      Point3f diff = v.P() - t.P();

      double dist2  = diff.SquaredNorm();
//       double len = sqrt(dist2);
//       if(len <= 0.001 * radius) len = radius*0.001;

      double w = exp(dist2*iradius16);
      //double rep = w * pow(1.0 / len, repulsion_power);

      if (need_density)
      {
        w *= samples_density[t.m_index];
      }

      //w = 1.0;

      samples_average[i] += t.P() * w;  

//       if (i < 5)
//       {
//         cout << t.P().X() << "  " << t.P().Y() << " " << t.P().Z() << endl;
//       }
      samples_average_weight_sum[i] += w;
    }
  }
}

void WLOP::computeDensity(bool isOriginal, double radius)
{
	CMesh* mesh;
	if (isOriginal)
	{
		mesh = original;
	}
	else
	{
		mesh = samples;
	}

	double radius2 = radius * radius;
	double iradius16 = -para->getDouble("H Gaussian Para") / radius2;

	for(int i = 0; i < mesh->vert.size(); i++)
	{
		CVertex& v = mesh->vert[i];

		if (isOriginal)
		{
			original_density[i] = 1.;
		}
		else
		{
			samples_density[i] = 1.;
		}

		vector<int>* neighbors = &v.neighbors;

		for (int j = 0; j < neighbors->size(); j++)
		{
			CVertex& t = mesh->vert[(*neighbors)[j]];
			double dist2  = (v.P() - t.P()).SquaredNorm();
			double den = exp(dist2*iradius16);

			if (isOriginal)
			{
				original_density[i] += den;
			}
			else
			{
				samples_density[i] += den;
			}
		}
	}

	for(int i = 0; i < mesh->vert.size(); i++)
	{
		if (isOriginal)
		{
			CVertex& v = mesh->vert[i];
			original_density[i] = 1. / original_density[i];
		}
		else
		{
			samples_density[i] = sqrt(samples_density[i]);
		}
	}

}

void WLOP::updateAdaptiveNeighbor()
{
  double radius = para->getDouble("CGrid Radius"); 
  double radius2 = radius * radius;

  for (int i = 0; i < samples->vert.size(); i++)
  {
    CVertex& v = samples->vert[i];

    vector<int> new_neighbors;
    for (int j = 0; j < v.neighbors.size(); j++)
    {
      CVertex& t = samples->vert[v.neighbors[j]];

      float dist2 = GlobalFun::computeEulerDistSquare(v.P(), t.P());
      //Point3f diff = t.P() - v.P();
      //diff.normalized();

      if (dist2 < radius2 && (t.N() * v.N()) > 0)
      {
        new_neighbors.push_back(v.neighbors[j]);
      }
    }

    v.neighbors.clear();
    for (int j = 0; j < new_neighbors.size(); j++)
    {
      v.neighbors.push_back(new_neighbors[j]);
    }
  }
}

double WLOP::iterate()
{
  use_adaptive_mu = para->getBool("Use Adaptive Mu");
  is_sample_close_to_original.assign(samples->vert.size(), false);
  
	Timer time;

	initVertexes();

	time.start("Sample Original Neighbor Tree!!!");
	GlobalFun::computeBallNeighbors(samples, original, 
		para->getDouble("CGrid Radius"), box);
	time.end();

  if (!para->getBool("Use Adaptive Sample Neighbor"))
  {
    time.start("Sample Sample Neighbor Tree");
    GlobalFun::computeBallNeighbors(samples, NULL, 
      para->getDouble("CGrid Radius"), samples->bbox);
    time.end();
  }
  else
  {
    updateAdaptiveNeighbor();
  }

	
	if (nTimeIterated == 0) 
	{
		if (para->getBool("Need Compute Density"))
		{
			double local_density_para = 0.95;
			time.start("Original Original Neighbor Tree");
			GlobalFun::computeBallNeighbors(original, NULL, 
				para->getDouble("CGrid Radius") * local_density_para, original->bbox);
			time.end();

			time.start("Compute Original Density");
			original_density.assign(original->vn, 0);

			computeDensity(true, para->getDouble("CGrid Radius") * local_density_para);
			time.end();
		}
		
	}

	if (para->getBool("Need Compute Density"))
	{
		time.start("Compute Density For Sample");
		computeDensity(false, para->getDouble("CGrid Radius"));
		time.end();
	}

	time.start("Sample Original Neighbor Tree!!!");
	GlobalFun::computeBallNeighbors(samples, original, 
		para->getDouble("CGrid Radius"), box);
	time.end();

  time.start("Compute Average Term");
  if (para->getBool("Original Combine Sample"))
  {
    computeAverageAddSampleTerm(samples, original);
  }
  else
  {
    computeAverageTerm(samples, original);
  }
	time.end();

	time.start("Compute Repulsion Term");
	computeRepulsionTerm(samples);
	time.end();

  bool need_sample_average_term = false;
  if (para->getBool("Need Sample Average"))
  {
    time.start("Compute Repulsion Term");
    computeSampleAverageTerm(samples);
    time.end();
    need_sample_average_term = true;
  }

	double mu = para->getDouble("Repulsion Mu");
  double mu3 = para->getDouble("Sample Average Mu3");

  double current_mu = para->getDouble("Repulsion Mu");

	Point3f c;

  vector<Point3f> new_sample_positions;
  vector<float> move_proj_vec;

  double radius = para->getDouble("CGrid Radius");
  double radius2 = radius * radius;
  double iradius16 = -para->getDouble("H Gaussian Para")/radius2;

  if (use_adaptive_mu)
  {
    for (int i = 0; i < samples->vert.size(); i++)
    {
      if (is_sample_close_to_original[i])
      {
        samples->vert[i].is_fixed_sample = true;
      }
      else
      {
        samples->vert[i].is_fixed_sample = false;
      }
    }
  }

  if (para->getBool("Need Averaging Movement"))
  {
    new_sample_positions.assign(samples->vert.size(), Point3f(0.,0.,0.));
    move_proj_vec.assign(samples->vert.size(), 0.);

    for (int i = 0; i < samples->vert.size(); i++)
    {
      CVertex& v = samples->vert[i];
      Point3f temp_p = v.P();

      if (average_weight_sum[i] > 1e-20)
      {
        temp_p = average[i] / average_weight_sum[i];
      }

      if (use_adaptive_mu)
      {
        if (is_sample_close_to_original[i])
        {
          current_mu = mu;
        }
        else
        {
          current_mu = mu3;
        }
      }

      if (repulsion_weight_sum[i] > 1e-20 && current_mu > 0)
      {
        temp_p += repulsion[i] * (current_mu / repulsion_weight_sum[i]);
      }

      Point3f move_vector = temp_p - v.P();
      float move_proj = move_vector * v.N();
      move_proj_vec[i] = move_proj;
    }

    for (int i = 0; i < samples->vert.size(); i++)
    {
      CVertex& v = samples->vert[i];

      float sum_move_proj = 0.0;
      float sum_w = 0.0;

      for (int j = 0; j < v.neighbors.size(); j++)
      {
        int neighbor_idx = v.neighbors[j];
        CVertex& t = samples->vert[neighbor_idx];
        float neighbor_move_proj = move_proj_vec[neighbor_idx];

        float dist2 = GlobalFun::computeEulerDistSquare(v.P(), t.P());
        float w = exp(dist2 * iradius16);

        sum_move_proj += w * neighbor_move_proj;
        sum_w += w;
      }

      float avg_move_proj = sum_move_proj / sum_w;

      if (sum_w > 0.0)
      {
        v.P() += v.N() * avg_move_proj;
      }

    }
  }
  else
  {
    for(int i = 0; i < samples->vert.size(); i++)
    {
      CVertex& v = samples->vert[i];
      c = v.P();

      if (average_weight_sum[i] > 1e-20)
      {
        v.P() = average[i] / average_weight_sum[i];
      }

      if (repulsion_weight_sum[i] > 1e-20 && mu > 0)
      {
        v.P() +=  repulsion[i] * (mu / repulsion_weight_sum[i]);
      }

      if (need_sample_average_term && samples_average_weight_sum[i] > 1e-20 && mu3 >= 0)
      {
        v.P() +=  samples_average[i] * (mu3 / samples_average_weight_sum[i]);
      }

      if (/*average_weight_sum[i] > 1e-20 && */repulsion_weight_sum[i] > 1e-20 )
      {
        Point3f diff = v.P() - c; 
        double move_error = sqrt(diff.SquaredNorm());
        error_x += move_error; 
      }
    }
    error_x = error_x / samples->vn;
  }


	para->setValue("Current Movement Error", DoubleValue(error_x));
	cout << "****finished compute WLOP error:	" << error_x << endl;

	if (para->getBool("Need Compute PCA"))
	{
		time.start("Recompute PCA");
		recomputePCA_Normal();
		time.end();
	}
	return error_x;
}


void WLOP::recomputePCA_Normal()
{
  CMesh temp_mesh;
  //if (para->getBool("Use Adaptive Sample Neighbor"))
  if (false)
  {

    //double sigma = global_paraMgr.norSmooth.getDouble("Sharpe Feature Bandwidth Sigma");
    //double radius = para->getDouble("CGrid Radius"); 

    //double radius2 = radius * radius;
    //double iradius16 = -4 / radius2;

    ////CMesh* samples = mesh;
    ////GlobalFun::computeBallNeighbors(samples, NULL, para->getDouble("CGrid Radius"), samples->bbox);

    //vector<Point3f> normal_sum;
    //vector<float> normal_weight_sum;

    //normal_sum.assign(samples->vert.size(), Point3f(0.,0.,0.));
    //normal_weight_sum.assign(samples->vert.size(), 0);

    //for(int i = 0; i < samples->vert.size(); i++)
    //{
    //  CVertex& v = samples->vert[i];

    //  for (int j = 0; j < v.neighbors.size(); j++)
    //  {
    //    CVertex& t = samples->vert[v.neighbors[j]];

    //    Point3f diff = v.P() - t.P();
    //    double dist2  = diff.SquaredNorm();

    //    double rep; 

    //    Point3f vm(v.N());
    //    Point3f tm(t.N());
    //    Point3f d = vm-tm;
    //    double psi = exp(-pow(1-vm*tm, 2)/pow(max(1e-8,1-cos(sigma/180.0*3.1415926)), 2));
    //    double theta = exp(dist2*iradius16);
    //    rep = psi * theta;
    //    rep = max(rep, 1e-10);

    //    normal_weight_sum[i] += rep;
    //    normal_sum[i] += tm * rep;         
    //  }

    //  if (normal_weight_sum[i] > 1e-6)
    //  {
    //    v.N() = normal_sum[i] / normal_weight_sum[i];
    //  }
    //}





    //for(int i = 0; i < samples->vn; i++)
    //{
    //  mesh_temp[i].P() = samples->vert[i].P();
    //}

    //int knn = global_paraMgr.norSmooth.getInt("PCA KNN");
    //vcg::NormalExtrapolation<vector<CVertex> >::ExtrapolateNormalsWithExistingNeighbor(mesh_temp.begin(), mesh_temp.end());

    //for(int i = 0; i < samples->vn; i++)
    //{
    //  Point3f& new_normal = mesh_temp[i].N();
    //  CVertex& v = samples->vert[i];
    //  if (v.N() * new_normal > 0)
    //  {
    //    v.N() = new_normal;
    //  }
    //  else
    //  {
    //    v.N() = -new_normal;
    //  }
    //  v.recompute_m_render();
    //}




    for(int i = 0; i < samples->vn; i++)
    { 
      temp_mesh.vert.push_back(samples->vert[i]);
    }
    temp_mesh.vn = samples->vert.size();

    GlobalFun::computeUndirectedNormal(&temp_mesh, para->getDouble("CGrid Radius"));
 
    for(int i = 0; i < samples->vn; i++)
    {
      Point3f& new_normal = temp_mesh.vert[i].N();
      //Point3f& new_normal = temp_mesh.vert[i].eigen_vector1;

      CVertex& v = samples->vert[i];
      if (v.N() * new_normal > 0)
      {
        v.N() = new_normal;
      }
      else
      {
        v.N() = -new_normal;
      }
      v.recompute_m_render();
    }
  }
  else
  {
    for(int i = 0; i < samples->vn; i++)
    {
      mesh_temp[i].P() = samples->vert[i].P();
    }

    int knn = global_paraMgr.norSmooth.getInt("PCA KNN");
    vcg::NormalExtrapolation<vector<CVertex> >::ExtrapolateNormals(mesh_temp.begin(), mesh_temp.end(), knn, -1);

    for(int i = 0; i < samples->vn; i++)
    {
      Point3f& new_normal = mesh_temp[i].N();
      CVertex& v = samples->vert[i];
      if (v.N() * new_normal > 0)
      {
        v.N() = new_normal;
      }
      else
      {
        v.N() = -new_normal;
      }
      v.recompute_m_render();
    }
  }

}

void WLOP::stepForward()
{
  float pace_size = 0.1 * para->getDouble("CGrid Radius");
  for (int i = 0; i < samples->vert.size(); i++)
  {
    CVertex& v = samples->vert[i];
    v.P() += v.N() * pace_size;
  }
  //recomputePCA_Normal();
}

void WLOP::computeInitialSampleNeighbor()
{
  
  Timer time;
  time.start("Sample Sample Neighbor Tree");
  GlobalFun::computeBallNeighbors(samples, NULL, para->getDouble("CGrid Radius"), samples->bbox);
  time.end();

}


void WLOP::runSkelWlop()
{
  cout << "runSkelWlop" << endl;

  Timer time;

  initVertexes();

  time.start("Samples Initial");
  GlobalFun::computeBallNeighbors(samples, NULL, para->getDouble("CGrid Radius"), samples->bbox);
  GlobalFun::computeEigenWithTheta(samples, para->getDouble("CGrid Radius") / sqrt(para->getDouble("H Gaussian Para")));
  time.end();

  if (nTimeIterated == 0) 
  {
    time.start("Original Initial");
    GlobalFun::computeBallNeighbors(original, NULL, 
      para->getDouble("CGrid Radius"), original->bbox);

    original_density.assign(original->vn, 0);
    if (para->getBool("Need Compute Density"))
    {
      computeDensity(true, para->getDouble("CGrid Radius"));
    }
    time.end();
  }

  time.start("Sample Original neighbor");
  GlobalFun::computeBallNeighbors(samples, original, 
    para->getDouble("CGrid Radius"), box);
  time.end();

  time.start("computeAverageTerm");
  computeAverageTerm(samples, original);
  time.end();

  time.start("computeRepulsionTerm");
  computeRepulsionTerm(samples);
  time.end();

  double min_sigma = GlobalFun::getDoubleMAXIMUM();
  double max_sigma = -1;
  for (int i = 0; i < samples->vn; i++)
  {
    CVertex& v = samples->vert[i];
    if (v.eigen_confidence < min_sigma)
    {
      min_sigma = v.eigen_confidence;
    }
    if (v.eigen_confidence > max_sigma)
    {
      max_sigma = v.eigen_confidence;
    }
  }

  double mu_max = para->getDouble("Repulsion Mu");
  double mu_min = para->getDouble("Repulsion Mu2");
  double mu_length = abs(mu_max - mu_min);
  double sigma_length = abs(max_sigma - min_sigma);
  Point3f c;
  int moving_num = 0;
  double max_error = 0;

  for(int i = 0; i < samples->vert.size(); i++)
  {
    CVertex& v = samples->vert[i];
    if (v.is_fixed_sample || v.is_skel_ignore)
    {
      continue;
    }
    c = v.P();

    double mu = (mu_length / sigma_length) * (v.eigen_confidence - min_sigma) + mu_min;

    if (average_weight_sum[i] > 1e-20)
    {
      v.P() = average[i] / average_weight_sum[i];

    }
    if (repulsion_weight_sum[i] > 1e-20 && mu >= 0)
    {
      v.P() +=  repulsion[i] * (mu / repulsion_weight_sum[i]);
    }

    if (average_weight_sum[i] > 1e-20 && repulsion_weight_sum[i] > 1e-20 )
    {
      moving_num++;
      Point3f diff = v.P() - c; 
      double move_error = sqrt(diff.SquaredNorm());

      error_x += move_error; 
    }
  }
  error_x = error_x / moving_num;

  para->setValue("Current Movement Error", DoubleValue(error_x));
  cout << "****finished compute Skeletonization error:	" << error_x << endl;
  return ;
}


void WLOP::runDragWlop()
{

}


void WLOP::runRegularizeSamples()
{
  cout << "runRegularizeSamples" << endl;

  GlobalFun::computeBallNeighbors(samples, NULL, para->getDouble("CGrid Radius"), samples->bbox);
  GlobalFun::computeEigenWithTheta(samples, para->getDouble("CGrid Radius") / sqrt(para->getDouble("H Gaussian Para")));

//   int branch_KNN = para->getDouble("Branch Search KNN");
//   GlobalFun::computeAnnNeigbhors(samples->vert, samples->vert, branch_KNN, false, "void Skeletonization::searchNewBranches()");

  vector<Point3f> new_sample_set;
  new_sample_set.assign(samples->vert.size(), Point3f(0, 0, 0));
  for (int i = 0; i < samples->vert.size(); i++)
  {
    CVertex& v = samples->vert[i];
    Point3f direction = v.eigen_vector0.Normalize();

    Point3f front_nearest_p = v.P();
    Point3f back_nearest_p = v.P();
    double front_nearest_dist = 1000000.;
    double back_nearest_dist = -1000000.;

    for (int j = 0; j < v.neighbors.size(); j++)
    {
      int neighbor_idx = v.neighbors[j];
      CVertex& t = samples->vert[neighbor_idx];

      Point3f diff = t.P() - v.P();
      double proj_dist = diff * direction;

      if (proj_dist > 0)
      {
        if (proj_dist < front_nearest_dist)
        {
          front_nearest_p = t.P();
          front_nearest_dist = proj_dist;
        }
      }
      else
      {
        if (proj_dist > back_nearest_dist)
        {
          back_nearest_p = t.P();
          back_nearest_dist = proj_dist;
        }
      }
    }

    if (front_nearest_dist > 100000 || back_nearest_dist < -100000)
    {
      cout << "end pointssss" << endl;
      continue;
    }

    v.P() = (front_nearest_p + back_nearest_p) / 2.0;
  }
}


void WLOP::computeDiskNeighborhood()
{
  GlobalFun::computeBallNeighbors(dual_samples, NULL, para->getDouble("CGrid Radius"), dual_samples->bbox);
  GlobalFun::computeBallNeighbors(samples, NULL, para->getDouble("CGrid Radius") * 1.5, samples->bbox);

  for (int i = 0; i < dual_samples->vert.size(); i++)
  {
    CVertex& dual_v = dual_samples->vert[i];
    CVertex& v = samples->vert[i];

    v.N() = dual_v.N();
  }

  vector<NeighborDisk> neighbor_disks;
  for (int i = 0; i < dual_samples->vert.size(); i++)
  {
    CVertex& dual_v = dual_samples->vert[i];
    NeighborDisk disk(dual_v.P(), dual_v.N());
    for (int j = 0; j < dual_v.neighbors.size(); j++)
    {
      disk.add_point(dual_samples->vert[dual_v.neighbors[j]]);
    }
    neighbor_disks.push_back(disk);
  }

  double angle = global_paraMgr.norSmooth.getDouble("Sharpe Feature Bandwidth Sigma");
  //double angle_threshold = angle * 3.1415926 / 180.;
  for (int i = 0; i < dual_samples->vert.size(); i++)
  {
    double occupy_percentage = neighbor_disks[i].getOccupyPercentage();
    if (occupy_percentage > 0.66)
      continue;

    CVertex& dual_v = dual_samples->vert[i];
    CVertex& v = samples->vert[i];

    vector<int> new_neighbors;
    for (int j = 0; j < v.neighbors.size(); j++)
    {
      CVertex& t = samples->vert[v.neighbors[j]];
      double normal_diff = GlobalFun::computeRealAngleOfTwoVertor(t.N(), v.N()); 
      
      //new_neighbors.push_back(v.neighbors[j]);

      if (normal_diff < angle)
      {
        new_neighbors.push_back(v.neighbors[j]);
      }
    }

    for (int j = 0; j < new_neighbors.size(); j++)
    {
      dual_v.neighbors.push_back(new_neighbors[j]);
    }
  }
}
