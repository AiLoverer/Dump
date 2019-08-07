<?php

return array(
    'frame1'=>array(        
		'thread_id'=>1,
		'addr_pc'=>1000000000,
		'module_id'=>1,
		'offs_in_module'=>123,
		'symbol_name'=>'?CMainDlg@OnCrash@@QAEEPZ',
		'und_symbol_name'=>'CMainDlg::OnCrash()',
		'offs_in_symbol'=>1,
		'src_file_name'=>'CrashRptTest.cpp',
		'src_line'=>1023,
		'offs_in_line'=>10		
    ),	
	
	'frame2'=>array(        
		'thread_id'=>1,
		'addr_pc'=>20434032323424,
		'module_id'=>2,
		'offs_in_module'=>12,
		'symbol_name'=>'',
		'und_symbol_name'=>'',
		'offs_in_symbol'=>1245,
		'src_file_name'=>'',
		'src_line'=>0,
		'offs_in_line'=>0		
    ),	
	
	'frame3'=>array(        
		'thread_id'=>1,
		'addr_pc'=>30232002323020,
		'module_id'=>1,
		'offs_in_module'=>1223432,
		'symbol_name'=>'__WinMainCrtStartup@12',
		'und_symbol_name'=>'_WinMainCRTStartup()',
		'offs_in_symbol'=>11225,
		'src_file_name'=>'crtex.c',
		'src_line'=>120,
		'offs_in_line'=>1		
    ),	
    
    'frame4'=>array(        
		'thread_id'=>3,
		'addr_pc'=>30232002323020,
		'module_id'=>1,
		'offs_in_module'=>1223432,
		'symbol_name'=>'wmain@12',
		'und_symbol_name'=>'wmain()',
		'offs_in_symbol'=>11225,
		'src_file_name'=>'crtex.c',
		'src_line'=>120,
		'offs_in_line'=>1		
    ),
    
    'frame5'=>array(        
		'thread_id'=>4,
		'addr_pc'=>30232002323020,
		'module_id'=>1,
		'offs_in_module'=>1223432,		
    ),
);

