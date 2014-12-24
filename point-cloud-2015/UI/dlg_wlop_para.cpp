#include "UI/dlg_wlop_para.h"

WlopParaDlg::WlopParaDlg(QWidget *p, ParameterMgr * _paras, GLArea * _area) : QFrame(p)
{
	ui = new Ui::para_wlop;
	WlopParaDlg::ui->setupUi(this);
	m_paras = _paras;
	area = _area;

	if(!initWidgets())
	{
		cerr << "Warning:  WlopParaDlg::initWidgets failed!" << endl;
		 return ;
	}

	run_backward_first = false;

	initConnects();
}


void WlopParaDlg::initConnects()
{
	if (!connect(area,SIGNAL(needUpdateStatus()),this,SLOT(initWidgets())))
	{
		cout << "can not connect signal" << endl;
	}

	if(!connect(ui->radius,SIGNAL(valueChanged(double)),this,SLOT(getRadiusValues(double))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if (!connect(ui->dual_radius, SIGNAL(valueChanged(double)), this, SLOT(getDualRadius(double))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}

	if (!connect(ui->increasing_step_size, SIGNAL(valueChanged(double)), this, SLOT(get_increasing_step_size(double))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if (!connect(ui->local_neighbor_size, SIGNAL(valueChanged(double)), this, SLOT(get_local_neighbor_size(double))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if (!connect(ui->local_angle_threshold, SIGNAL(valueChanged(double)), this, SLOT(get_local_angle_threshold(double))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}

	//if(!connect(ui->rep_pow,SIGNAL(valueChanged(double)),this,SLOT(getRepPow(double))))
	//{
	//	cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	//}
	//if(!connect(ui->fit_pow,SIGNAL(valueChanged(double)),this,SLOT(getFitPow(double))))
	//{
	//	cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	//}
	if (!connect(ui->original_averaging_KNN, SIGNAL(valueChanged(double)), this, SLOT(getOriginalAverageKNN(double))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if(!connect(ui->mu,SIGNAL(valueChanged(double)),this,SLOT(getMu(double))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
  if(!connect(ui->sample_average_mu3,SIGNAL(valueChanged(double)),this,SLOT(getMu3(double))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }
	if(!connect(ui->iter,SIGNAL(valueChanged(int)),this,SLOT(getIter(int))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if(!connect(ui->compute_density,SIGNAL(clicked(bool)),this,SLOT(isDensity(bool))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if(!connect(ui->compute_pca,SIGNAL(clicked(bool)),this,SLOT(isPca(bool))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
  if(!connect(ui->Need_sample_average,SIGNAL(clicked(bool)),this,SLOT(needSampleAverage(bool))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }
  if(!connect(ui->Need_original_combine_sample,SIGNAL(clicked(bool)),this,SLOT(needOriginalCombineSample(bool))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }
  if(!connect(ui->Need_averaging_movement,SIGNAL(clicked(bool)),this,SLOT(needAverageMovement(bool))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }

  if(!connect(ui->Use_Elliptical_Original_Neighbor,SIGNAL(clicked(bool)),this,SLOT(useEllipticalOriginalNeighbor(bool))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }
  if(!connect(ui->Use_KNN_Sample_Neighbor,SIGNAL(clicked(bool)),this,SLOT(useAdaptiveSampleNeighbor(bool))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }
  if(!connect(ui->Use_Adaptive_Mu,SIGNAL(clicked(bool)),this,SLOT(useAdaptiveMu(bool))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }
  if(!connect(ui->Use_tangent_vector,SIGNAL(clicked(bool)),this,SLOT(useTangentVector(bool))))
  {
    cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
  }
	if (!connect(ui->Use_confidence, SIGNAL(clicked(bool)), this, SLOT(useConfidence(bool))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if (!connect(ui->Use_original_Averaging_KNN, SIGNAL(clicked(bool)), this, SLOT(useOriginalKNN(bool))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if (!connect(ui->Use_kite_points, SIGNAL(clicked(bool)), this, SLOT(useKitePoints(bool))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	if (!connect(ui->run_backward_first, SIGNAL(clicked(bool)), this, SLOT(useBackwardFirst(bool))))
	{
		cerr << "cannot connect WlopParaDlg::getDoubleValues(double)." << endl;
	}
	//
	if(!connect(ui->wlop_apply,SIGNAL(clicked()),this,SLOT(applyWlop())))
	{
		cerr << "cannot connect WlopParaDlg::applyWlop()." << endl;
	}
  if(!connect(ui->dual_wlop_apply,SIGNAL(clicked()),this,SLOT(applyDualWlop())))
  {
    cerr << "cannot connect WlopParaDlg::applyDualWlop()." << endl;
  }

	connect(ui->anisotropic_lop_apply,SIGNAL(clicked()),this,SLOT(applyAnisotropicLop()));
  connect(ui->step_forward,SIGNAL(clicked()),this,SLOT(applyStepForward()));
  connect(ui->dual_connection,SIGNAL(clicked()),this,SLOT(applyDualConnection()));

  connect(ui->skel_wlop,SIGNAL(clicked()),this,SLOT(applySkelWlop()));
  connect(ui->dual_drag_wlop,SIGNAL(clicked()),this,SLOT(applyDragWlop()));
  connect(ui->regularize_samples,SIGNAL(clicked()),this,SLOT(applyRegularizeSamples()));
  connect(ui->regularize_normals,SIGNAL(clicked()),this,SLOT(applyRegularizeNormals()));

  connect(ui->wlop_projection,SIGNAL(clicked()),this,SLOT(applyProjection()));
	connect(ui->mat_lop, SIGNAL(clicked()), this, SLOT(applyMatLOP()));

	connect(ui->compute_confidence, SIGNAL(clicked()), this, SLOT(applyComputeConfidence()));
	connect(ui->compute_distribution, SIGNAL(clicked()), this, SLOT(applyComputeDistribution()));
	connect(ui->inner_clustering, SIGNAL(clicked()), this, SLOT(applyComputeInnerClustering()));

	connect(ui->show_pick_distribution, SIGNAL(clicked()), this, SLOT(applyShowPickDistribution()));
	connect(ui->compute_correspondence, SIGNAL(clicked()), this, SLOT(applyComputeCorrespondence()));

	connect(ui->detect_kite_points, SIGNAL(clicked()), this, SLOT(applyDetectKitePoints()));


	connect(ui->pushButton_progressive_neighborhood, SIGNAL(clicked()), this, SLOT(applyProgressiveNeighborhood()));
	connect(ui->pushButton_ellipsoid_fitting, SIGNAL(clicked()), this, SLOT(applyEllipsoidFitting()));

	connect(ui->inner_points_classification, SIGNAL(clicked()), this, SLOT(applyInnerPointsClassification()));


	connect(ui->pushButton_search_neighborhood, SIGNAL(clicked()), this, SLOT(applySearchNeighborhood()));
	connect(ui->pushButton_smooth_neighborhood, SIGNAL(clicked()), this, SLOT(applySmoothNeighborhood()));
	connect(ui->pushButton_regularize_inner_points, SIGNAL(clicked()), this, SLOT(applyInnerPointsRegularization()));

	connect(ui->move_backward, SIGNAL(clicked()), this, SLOT(applyMoveBackward()));
	connect(ui->self_WLOP, SIGNAL(clicked()), this, SLOT(applySelfWLOP()));
	connect(ui->normal_smoothing, SIGNAL(clicked()), this, SLOT(applyNormalSmoothing()));

	connect(ui->self_PCA, SIGNAL(clicked()), this, SLOT(applySelfPCA()));
	connect(ui->self_projection, SIGNAL(clicked()), this, SLOT(applySelfPorjection()));

	connect(ui->compute_initial_neighborhood, SIGNAL(clicked()), this, SLOT(applyComputeInitialNeighborhood()));
}


bool WlopParaDlg::initWidgets()
{
	ui->radius->setValue(m_paras->wLop.getDouble("CGrid Radius"));
	ui->mu->setValue(m_paras->wLop.getDouble("Repulsion Mu"));
	/*ui->rep_pow->setValue(m_paras->wLop.getDouble("Repulsion Power"));
	ui->fit_pow->setValue(m_paras->wLop.getDouble("Average Power"));*/
	ui->iter->setValue(m_paras->wLop.getDouble("Num Of Iterate Time"));
  ui->sample_average_mu3->setValue(m_paras->wLop.getDouble("Dual Mu3"));
	ui->original_averaging_KNN->setValue(m_paras->wLop.getDouble("Original Averaging KNN"));
	ui->dual_radius->setValue(m_paras->wLop.getDouble("Dual Radius"));

	ui->increasing_step_size->setValue(m_paras->wLop.getDouble("Increasing Step Size"));
	ui->local_neighbor_size->setValue(m_paras->wLop.getDouble("Local Neighbor Size"));
	ui->local_angle_threshold->setValue(m_paras->wLop.getDouble("Local Angle Threshold"));

	
	Qt::CheckState state = m_paras->wLop.getBool("Need Compute Density") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
	ui->compute_density->setCheckState(state);

	state = m_paras->wLop.getBool("Need Compute PCA") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
	ui->compute_pca->setCheckState(state);
// 
//   state = m_paras->wLop.getBool("Need Sample Average") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
//   ui->Need_sample_average->setCheckState(state);

	state = m_paras->wLop.getBool("Need Similarity") ? (Qt::CheckState::Checked) : (Qt::CheckState::Unchecked);
	ui->Need_sample_average->setCheckState(state);

  state = m_paras->wLop.getBool("Original Combine Sample") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
  ui->Need_original_combine_sample->setCheckState(state);

  state = m_paras->wLop.getBool("Need Averaging Movement") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
  ui->Need_averaging_movement->setCheckState(state);

  state = m_paras->wLop.getBool("Use Elliptical Original Neighbor") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
  ui->Use_Elliptical_Original_Neighbor->setCheckState(state);

  state = m_paras->wLop.getBool("Use Adaptive Sample Neighbor") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
  ui->Use_KNN_Sample_Neighbor->setCheckState(state);

  state = m_paras->wLop.getBool("Use Adaptive Mu") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
  ui->Use_Adaptive_Mu->setCheckState(state);

  state = m_paras->wLop.getBool("Use Tangent Vector") ? (Qt::CheckState::Checked): (Qt::CheckState::Unchecked);
  ui->Use_tangent_vector->setCheckState(state);

	state = m_paras->wLop.getBool("Use Confidence") ? (Qt::CheckState::Checked) : (Qt::CheckState::Unchecked);
	ui->Use_confidence->setCheckState(state);

	state = m_paras->wLop.getBool("Use Original Averaging KNN") ? (Qt::CheckState::Checked) : (Qt::CheckState::Unchecked);
	ui->Use_original_Averaging_KNN->setCheckState(state);

	state = m_paras->wLop.getBool("Use Kite Points") ? (Qt::CheckState::Checked) : (Qt::CheckState::Unchecked);
	ui->Use_kite_points->setCheckState(state);


	update();
	repaint();
	return true;
}

void WlopParaDlg::setFrameConent()
{
	if(layout()) delete layout();
	QGridLayout * vLayout = new QGridLayout(this);
	vLayout->setAlignment(Qt::AlignTop);
	setLayout(vLayout);

	showNormal();
	adjustSize();
}

void WlopParaDlg::getRadiusValues(double _val)
{
	m_paras->setGlobalParameter("CGrid Radius",DoubleValue(_val));
	area->updateGL();
}


void WlopParaDlg::getRepPow(double _val)
{
	m_paras->wLop.setValue("Repulsion Power",DoubleValue(_val));
}

void WlopParaDlg::getFitPow(double _val)
{
	m_paras->wLop.setValue("Average Power",DoubleValue(_val));
}

void WlopParaDlg::getIter(int _val)
{
	m_paras->wLop.setValue("Num Of Iterate Time",DoubleValue(_val));
}

void WlopParaDlg::getMu(double _val)
{
	m_paras->wLop.setValue("Repulsion Mu", DoubleValue(_val));
}

void WlopParaDlg::getMu3(double _val)
{
  m_paras->wLop.setValue("Dual Mu3", DoubleValue(_val));
}

void WlopParaDlg::getOriginalAverageKNN(double _val)
{
	m_paras->wLop.setValue("Original Averaging KNN", DoubleValue(_val));
}

void WlopParaDlg::getDualRadius(double _val)
{
	m_paras->wLop.setValue("Dual Radius", DoubleValue(_val));
}


void WlopParaDlg::get_increasing_step_size(double _val)
{
	m_paras->wLop.setValue("Increasing Step Size", DoubleValue(_val));
}

void WlopParaDlg::get_local_neighbor_size(double _val)
{
	m_paras->wLop.setValue("Local Neighbor Size", DoubleValue(_val));
}

void WlopParaDlg::get_local_angle_threshold(double _val)
{
	m_paras->wLop.setValue("Local Angle Threshold", DoubleValue(_val));
}



void WlopParaDlg::isDensity(bool _val)
{
	m_paras->wLop.setValue("Need Compute Density",BoolValue(_val));
	if (_val)
	{
		area->wlop.setFirstIterate();
	}
}

 void WlopParaDlg::needSampleAverage(bool _val)
 {
   //m_paras->wLop.setValue("Need Sample Average", BoolValue(_val));
	 m_paras->wLop.setValue("Need Similarity", BoolValue(_val));
	 cout << "Need Similarity" << endl;
 }

void WlopParaDlg::isPca(bool _val)
{
	m_paras->wLop.setValue("Need Compute PCA",BoolValue(_val));
}

void WlopParaDlg::needOriginalCombineSample(bool _val)
{
  m_paras->wLop.setValue("Original Combine Sample", BoolValue(_val));
}

void WlopParaDlg::needAverageMovement(bool _val)
{
  m_paras->wLop.setValue("Need Averaging Movement", BoolValue(_val));
}

void WlopParaDlg::useEllipticalOriginalNeighbor(bool _val)
{
  m_paras->wLop.setValue("Use Elliptical Original Neighbor", BoolValue(_val));
}

void WlopParaDlg::useAdaptiveSampleNeighbor(bool _val)
{
  m_paras->wLop.setValue("Use Adaptive Sample Neighbor", BoolValue(_val));

  if (_val)
  {
    m_paras->wLop.setValue("Run Compute Initial Sample Neighbor", BoolValue(true));
    area->runWlop();
    m_paras->wLop.setValue("Run Compute Initial Sample Neighbor", BoolValue(false));
  }
}

void WlopParaDlg::useTangentVector(bool _val)
{
  m_paras->wLop.setValue("Use Tangent Vector", BoolValue(_val));
}

void WlopParaDlg::useConfidence(bool _val)
{
	m_paras->wLop.setValue("Use Confidence", BoolValue(_val));
}

void WlopParaDlg::useOriginalKNN(bool _val)
{
	m_paras->wLop.setValue("Use Original Averaging KNN", BoolValue(_val));
}

void WlopParaDlg::useKitePoints(bool _val)
{
	m_paras->wLop.setValue("Use Kite Points", BoolValue(_val));
}

void WlopParaDlg::useBackwardFirst(bool _val)
{
	run_backward_first = _val;
}

void WlopParaDlg::useAdaptiveMu(bool _val)
{
  m_paras->wLop.setValue("Use Adaptive Mu", BoolValue(_val));

  if (!_val)
  {
    CMesh* samples = area->dataMgr.getCurrentSamples();

    for (int i = 0; i < samples->vert.size(); i++)
    {
      samples->vert[i].is_fixed_sample = false;
    }
  }
}


// apply
void WlopParaDlg::applyWlop()
{
	Timer timer;
	timer.start("WWWWLLLLLOOOOOPPPP Time");
	area->runWlop();
	timer.end();

  //if (global_paraMgr.glarea.getBool("SnapShot Each Iteration"))
  //{
  //  area->runWlop();
  //}
  //else
  //{
  //  global_paraMgr.glarea.setValue("Running Algorithm Name", StringValue("WLOP"));
  //  calculation_thread.setArea(area);
  //  calculation_thread.start();
  //}

}

//void WlopParaDlg::applyDualConnection()
//{
//  CMesh* samples = area->dataMgr.getCurrentSamples();
//  CMesh* dual_samples = area->dataMgr.getCurrentDualSamples();
//
//  GlobalFun::computeAnnNeigbhors(samples->vert, dual_samples->vert, 5, false, "WlopParaDlg::applyDualConnection()");
//  //GlobalFun::computeAnnNeigbhors(dual_samples->vert, samples->vert, 5, false, "WlopParaDlg::applyDualConnection()");
//
//  cout << "finished ANN " << endl;
//
//  for (int i = 0; i < dual_samples->vert.size(); i++)
//  {
//    int dual_index = dual_samples->vert[i].neighbors[0];
//    if (dual_index < 0 && dual_index >= samples->vert.size())
//    {
//      dual_index = 0;
//    }
//    dual_samples->vert[i].dual_index = dual_index;
//  }
//
//  return;
//}


void WlopParaDlg::copySamplesToDualSamples()
{
	CMesh* samples = area->dataMgr.getCurrentSamples();
	CMesh* dual_samples = area->dataMgr.getCurrentDualSamples();

	dual_samples->vert.resize(samples->vert.size());
	for (int i = 0; i < samples->vert.size(); i++)
	{
		samples->vert[i].is_dual_sample = false;
		samples->vert[i].m_index = i;

		if (i < 20)
		{
			cout << samples->vert[i].skel_radius << endl;
		}
		//samples->vert[i].skel_radius = -1;


		dual_samples->vert[i] = samples->vert[i];
		dual_samples->vert[i].is_dual_sample = true;
		//dual_samples->vert[i].skel_radius = -1;

	}

}

void WlopParaDlg::applyDualConnection()
{
	copySamplesToDualSamples();

	CMesh* samples = area->dataMgr.getCurrentSamples();
	CMesh* dual_samples = area->dataMgr.getCurrentDualSamples();

	for (int i = 0; i < samples->vert.size(); i++)
	{
		CVertex& v = samples->vert[i];
		CVertex& dual_v = dual_samples->vert[i];
		//v.eigen_confidence = 0.0;
		//dual_v.eigen_confidence = 0.0;
		//v.skel_radius = 0.0;
		//dual_v.skel_radius = 0.0;
	}

// 	double temp_radius = global_paraMgr.wLop.getDouble("CGrid Radius") *0.5;
// 
// 	CMesh* dual_samples2 = area->dataMgr.getCurrentDualSamples();
// 	for (int i = 0; i < dual_samples2->vert.size(); i++)
// 	{
// 		CVertex& dual_v = dual_samples2->vert[i];
// 		dual_v.P() -= dual_v.N() * temp_radius;
// 	}


	return;


//   CMesh* samples = area->dataMgr.getCurrentSamples();
//   CMesh* dual_samples = area->dataMgr.getCurrentDualSamples();

  GlobalFun::computeAnnNeigbhors(samples->vert, dual_samples->vert, 1, false, "WlopParaDlg::applyDualConnection()");
  //GlobalFun::computeAnnNeigbhors(dual_samples->vert, samples->vert, 5, false, "WlopParaDlg::applyDualConnection()");

  cout << "finished ANN " << endl;

  //for (int i = 0; i < samples->vert.size(); i++)
  //{
  //  int dual_index = samples->vert[i].neighbors[0];
  //  if (dual_index < 0 && dual_index >= dual_samples->vert.size())
  //  {
  //    dual_index = 0;
  //  }
  //  samples->vert[i].dual_index = dual_index;
  //}

  //for (int i = 0; i < dual_samples->vert.size(); i++)
  //{
  //  int dual_index = dual_samples->vert[i].neighbors[0];
  //  if (dual_index < 0 && dual_index >= samples->vert.size())
  //  {
  //    dual_index = 0;
  //  }
  //  dual_samples->vert[i].dual_index = dual_index;
  //}

  for (int i = 0; i < dual_samples->vert.size(); i++)
  {
    dual_samples->vert[i].dual_index = i;
  }

  return;
}


 void WlopParaDlg::applyDualWlop()
 {
	 global_paraMgr.glarea.setValue("Algorithom Stop", BoolValue(false));
	 //applyWlop();
	 area->runWlop();


	 copySamplesToDualSamples();

 	 double temp_radius = global_paraMgr.wLop.getDouble("CGrid Radius");
 	 double dual_radius = global_paraMgr.wLop.getDouble("Dual Radius");
 	 global_paraMgr.setGlobalParameter("CGrid Radius", DoubleValue(dual_radius));
 	 //applySkelWlop();
	 m_paras->wLop.setValue("Run Skel WLOP", BoolValue(true));
	 area->runWlop();
	 m_paras->wLop.setValue("Run Skel WLOP", BoolValue(false));
 	 global_paraMgr.setGlobalParameter("CGrid Radius", DoubleValue(temp_radius));



   //double temp_radius = global_paraMgr.wLop.getDouble("CGrid Radius");
   //global_paraMgr.setGlobalParameter("CGrid Radius", DoubleValue(temp_radius));

   //m_paras->wLop.setValue("Run Dual WLOP", BoolValue(true));
   //area->runWlop();
   //m_paras->wLop.setValue("Run Dual WLOP", BoolValue(false));

   //global_paraMgr.setGlobalParameter("CGrid Radius", DoubleValue(temp_radius));


   //if (global_paraMgr.glarea.getBool("SnapShot Each Iteration"))
   //{
   //  m_paras->wLop.setValue("Run Dual WLOP", BoolValue(true));
   //  area->runWlop();
   //  m_paras->wLop.setValue("Run Dual WLOP", BoolValue(false));
   //}
   //else
   //{ 
   //  m_paras->wLop.setValue("Run Dual WLOP", BoolValue(true));
   //  global_paraMgr.glarea.setValue("Running Algorithm Name", StringValue("WLOP"));
   //  calculation_thread.setArea(area);
   //  calculation_thread.start();
   //}
 }

 void WlopParaDlg::applySkelWlop()
 {
	 m_paras->wLop.setValue("Run Skel WLOP", BoolValue(true));
	 area->runWlop();
	 m_paras->wLop.setValue("Run Skel WLOP", BoolValue(false));

	 //m_paras->wLop.setValue("Run Skel WLOP", BoolValue(true));
	 //global_paraMgr.glarea.setValue("Running Algorithm Name", StringValue("WLOP"));
	 //calculation_thread.setArea(area);
	 //calculation_thread.start();
	 
 }

 void WlopParaDlg::applyMatLOP()
 {
	 m_paras->wLop.setValue("Run MAT LOP", BoolValue(true));
	 area->runWlop();
	 m_paras->wLop.setValue("Run MAT LOP", BoolValue(false));

	 CMesh* samples = area->dataMgr.getCurrentSamples();

// 	 for (int i = 0; i < samples->vert.size(); i++)
// 	 {
// 		 CVertex& v = samples->vert[i];
// 
// 		 if (i < 50)
// 		 {
// 			 cout << "t t: " << v.eigen_confidence << endl;
// 		 }
// 	 }
 }

void WlopParaDlg::applyAnisotropicLop()
{
	if (global_paraMgr.glarea.getBool("SnapShot Each Iteration"))
	{
		m_paras->wLop.setValue("Run Anisotropic LOP", BoolValue(true));
		area->runWlop();
		m_paras->wLop.setValue("Run Anisotropic LOP", BoolValue(false));
	}
	else
	{
		m_paras->wLop.setValue("Run Anisotropic LOP", BoolValue(true));
		area->runWlop();
		m_paras->wLop.setValue("Run Anisotropic LOP", BoolValue(false));

// 		m_paras->wLop.setValue("Run Anisotropic LOP", BoolValue(true));
// 		global_paraMgr.glarea.setValue("Running Algorithm Name", StringValue("WLOP"));
// 		calculation_thread.setArea(area);
// 		calculation_thread.start();
	}

}

void WlopParaDlg::applyStepForward()
{
  m_paras->wLop.setValue("Run Step Forward", BoolValue(true));
  area->runWlop();
  m_paras->wLop.setValue("Run Step Forward", BoolValue(false));
}



void WlopParaDlg::applyDragWlop()
{
	Point3f shift_direction = Point3f(0.0, 0.0, 1.0);
	//Point3f shift_direction = Point3f(0.0, 1.0, 0.0);

	double step_size = 0.1;

	CMesh* target_samples = area->dataMgr.getCurrentTargetSamples();
	CMesh* target_dual_samples = area->dataMgr.getCurrentTargetDualSamples();

	for (int i = 0; i < target_samples->vert.size(); i++)
	{
		target_samples->vert[i].P() += shift_direction * step_size;
		target_dual_samples->vert[i].P() += shift_direction * step_size;
	}

//   m_paras->wLop.setValue("Run Dual Drag WLOP", BoolValue(true));
//   area->runWlop();
//   m_paras->wLop.setValue("Run Dual Drag WLOP", BoolValue(false));
}

void WlopParaDlg::applyRegularizeSamples()
{
  m_paras->wLop.setValue("Run Regularize Samples", BoolValue(true));
  area->runWlop();
  m_paras->wLop.setValue("Run Regularize Samples", BoolValue(false));
}

void WlopParaDlg::applyRegularizeNormals()
{
  m_paras->wLop.setValue("Run Regularize Normals", BoolValue(true));
  area->runWlop();
  m_paras->wLop.setValue("Run Regularize Normals", BoolValue(false));

	area->dataMgr.recomputeQuad();
}

void WlopParaDlg::applyDetectKitePoints()
{
	m_paras->wLop.setValue("Run Detect Kite Points", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Detect Kite Points", BoolValue(false));
}

void WlopParaDlg::applyProjection()
{
  m_paras->wLop.setValue("Run Projection", BoolValue(true));
  area->runWlop();
  m_paras->wLop.setValue("Run Projection", BoolValue(false));
}

WlopParaDlg::~WlopParaDlg()
{
	cout << "De-construct WlopParaDlg Frame." << endl;
	delete ui;
	ui = NULL;
}

void WlopParaDlg::applyNormalReform()
{
  m_paras->wLop.setValue("Run Normal Reform", BoolValue(true));
  area->runWlop();
  m_paras->wLop.setValue("Run Normal Reform", BoolValue(false));
}


void WlopParaDlg::applyComputeConfidence()
{
	m_paras->wLop.setValue("Run Compute Confidence", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Compute Confidence", BoolValue(false));
}

void WlopParaDlg::applyComputeDistribution()
{
	m_paras->wLop.setValue("Run Compute Distribution", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Compute Distribution", BoolValue(false));
}


void WlopParaDlg::applyComputeInnerClustering()
{
	m_paras->wLop.setValue("Run Inner Clustering", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Inner Clustering", BoolValue(false));
}

void WlopParaDlg::applyComputeCorrespondence()
{
	m_paras->wLop.setValue("Run Compute Correspondence", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Compute Correspondence", BoolValue(false));
}


void WlopParaDlg::applyShowPickDistribution()
{
	m_paras->wLop.setValue("Run Show Pick Distribution", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Show Pick Distribution", BoolValue(false));
}

void WlopParaDlg::applyProgressiveNeighborhood()
{
	m_paras->wLop.setValue("Run Progressive Neighborhood", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Progressive Neighborhood", BoolValue(false));
}

void WlopParaDlg::applyInnerPointsClassification()
{
	m_paras->wLop.setValue("Run Inner Points Classification", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Inner Points Classification", BoolValue(false));
}

void WlopParaDlg::applyEllipsoidFitting()
{
	m_paras->wLop.setValue("Run Ellipsoid Fitting", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Ellipsoid Fitting", BoolValue(false));
}


void WlopParaDlg::applyInnerPointsRegularization()
{
	m_paras->wLop.setValue("Run Inner Points Regularization", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Inner Points Regularization", BoolValue(false));
}

void WlopParaDlg::applySearchNeighborhood()
{
	m_paras->wLop.setValue("Run Search Neighborhood", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Search Neighborhood", BoolValue(false));
}

void WlopParaDlg::applySmoothNeighborhood()
{
	m_paras->wLop.setValue("Run Smooth Neighborhood", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Smooth Neighborhood", BoolValue(false));
}

void WlopParaDlg::applyMoveBackward()
{
	m_paras->wLop.setValue("Run Move Backward", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Move Backward", BoolValue(false));
}

void WlopParaDlg::applySelfWLOP()
{
	if (run_backward_first)
	{
		m_paras->wLop.setValue("Run Move Backward", BoolValue(true));
		area->runWlop();
		m_paras->wLop.setValue("Run Move Backward", BoolValue(false));

		m_paras->wLop.setValue("Run Self Projection", BoolValue(true));
		area->runWlop();
		m_paras->wLop.setValue("Run Self Projection", BoolValue(false));
	}

 
//     	m_paras->wLop.setValue("Run Normal Smoothing", BoolValue(true));
//     	area->runWlop();
//     	m_paras->wLop.setValue("Run Normal Smoothing", BoolValue(false));

// 		m_paras->wLop.setValue("Run Self PCA", BoolValue(true));
// 		area->runWlop();
// 		m_paras->wLop.setValue("Run Self PCA", BoolValue(false));
// 
//   	m_paras->wLop.setValue("Run Self Projection", BoolValue(true));
//   	area->runWlop();
//   	m_paras->wLop.setValue("Run Self Projection", BoolValue(false));



	m_paras->wLop.setValue("Run Self WLOP", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Self WLOP", BoolValue(false));
}

void WlopParaDlg::applyNormalSmoothing()
{
	if (run_backward_first)
	{
		m_paras->wLop.setValue("Run Move Backward", BoolValue(true));
		area->runWlop();
		m_paras->wLop.setValue("Run Move Backward", BoolValue(false));
	}

	m_paras->wLop.setValue("Run Normal Smoothing", BoolValue(true));
	area->runWlop();
 	m_paras->wLop.setValue("Run Normal Smoothing", BoolValue(false));
}

void WlopParaDlg::applySelfPCA()
{
	if (run_backward_first)
	{
		m_paras->wLop.setValue("Run Move Backward", BoolValue(true));
		area->runWlop();
		m_paras->wLop.setValue("Run Move Backward", BoolValue(false));
	}

	m_paras->wLop.setValue("Run Self PCA", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Self PCA", BoolValue(false));
}

void WlopParaDlg::applySelfPorjection()
{
	if (run_backward_first)
	{
		m_paras->wLop.setValue("Run Move Backward", BoolValue(true));
		area->runWlop();
		m_paras->wLop.setValue("Run Move Backward", BoolValue(false));
	}

	m_paras->wLop.setValue("Run Self Projection", BoolValue(true));
	area->runWlop();
	m_paras->wLop.setValue("Run Self Projection", BoolValue(false));
}

void WlopParaDlg::applyComputeInitialNeighborhood()
{
 	m_paras->wLop.setValue("Run Compute Initial Neighborhood", BoolValue(true));
 	area->runWlop();
 	m_paras->wLop.setValue("Run Compute Initial Neighborhood", BoolValue(false));
}

