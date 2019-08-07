<?php

class m120515_122132_initial extends CDbMigration
{
	private function q($subject)
	{
		// Crossplatform quote
		$q = '"';
		if($this->dbConnection->driverName=='mysql')
			$q = '`';
		
		return str_replace('"', $q, $subject);
	}
	
	public function up()
	{
		// Create {{lookup}} table
		$this->createTable('{{lookup}}',
					array(
						'id'=>'pk',
						//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
						'name'=>'VARCHAR(128) NOT NULL',
						'code'=>'INTEGER NOT NULL',
						'type'=>'VARCHAR(128) NOT NULL',
						'position'=>'INTEGER NOT NULL',
					)
				);
		
		// Insert crash report statuses
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Waiting',
					'type'=>'CrashReportStatus',
					'code'=>1,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Processing',
					'type'=>'CrashReportStatus',
					'code'=>2,
					'position'=>2,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Ready',
					'type'=>'CrashReportStatus',
					'code'=>3,
					'position'=>3,
				));

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Invalid',
					'type'=>'CrashReportStatus',
					'code'=>4,
					'position'=>4,
				));

		// Insert debug info statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Waiting',
					'type'=>'DebugInfoStatus',
					'code'=>1,
					'position'=>1,
				));

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Processing',
					'type'=>'DebugInfoStatus',
					'code'=>2,
					'position'=>2,
				));

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Ready',
					'type'=>'DebugInfoStatus',
					'code'=>3,
					'position'=>3,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Invalid',
					'type'=>'DebugInfoStatus',
					'code'=>4,
					'position'=>4,
				));

		// Insert user statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Active',
					'type'=>'UserStatus',
					'code'=>1,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Disabled',
					'type'=>'UserStatus',
					'code'=>2,
					'position'=>2,
				));

		// Insert user group statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Active',
					'type'=>'UserGroupStatus',
					'code'=>1,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Disabled',
					'type'=>'UserGroupStatus',
					'code'=>2,
					'position'=>2,
				));

		// Insert project statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Active',
					'type'=>'ProjectStatus',
					'code'=>1,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Disabled',
					'type'=>'ProjectStatus',
					'code'=>2,
					'position'=>2,
				));
		
		// Insert operation statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Started',
					'type'=>'OperationStatus',
					'code'=>1,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Succeeded',
					'type'=>'OperationStatus',
					'code'=>2,
					'position'=>2,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Failed',
					'type'=>'OperationStatus',
					'code'=>3,
					'position'=>3,
				));
		
		// Insert symbol load statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'No symbols loaded.',
					'type'=>'SymLoadStatus',
					'code'=>0,
					'position'=>0,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Symbols loaded.',
					'type'=>'SymLoadStatus',
					'code'=>1,
					'position'=>1,
				));
		
		// Insert bug statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'New',
					'type'=>'BugStatus',
					'code'=>1,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Reviewed',
					'type'=>'BugStatus',
					'code'=>2,
					'position'=>2,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Accepted',
					'type'=>'BugStatus',
					'code'=>3,
					'position'=>3,
				));

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Started',
					'type'=>'BugStatus',
					'code'=>4,
					'position'=>4,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Fixed',
					'type'=>'BugStatus',
					'code'=>101,
					'position'=>5,
				));

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Verified',
					'type'=>'BugStatus',
					'code'=>102,
					'position'=>6,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Duplicate',
					'type'=>'BugStatus',
					'code'=>103,
					'position'=>7,
				));

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'WontFix',
					'type'=>'BugStatus',
					'code'=>104,
					'position'=>8,
				));
		
		// Insert bug priority statuses

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Low',
					'type'=>'BugPriority',
					'code'=>1,
					'position'=>2,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Medium',
					'type'=>'BugPriority',
					'code'=>2,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'High',
					'type'=>'BugPriority',
					'code'=>3,
					'position'=>3,
				));

		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Critical',
					'type'=>'BugPriority',
					'code'=>4,
					'position'=>4,
				));

		// Insert bug reproducability statuses
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'NotTried',
					'type'=>'BugReproducability',
					'code'=>1,
					'position'=>1,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Never',
					'type'=>'BugReproducability',
					'code'=>2,
					'position'=>2,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Sometimes',
					'type'=>'BugReproducability',
					'code'=>3,
					'position'=>3,
				));
		
		$this->insert('{{lookup}}', 	
				array(
					'name'=>'Always',
					'type'=>'BugReproducability',
					'code'=>4,
					'position'=>4,
				));

		// Create {{usergroup}} table
		$this->createTable('{{usergroup}}',
					array(
						'id'=>'pk',
						//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
						'name'=>'VARCHAR(32) NOT NULL',
						'description'=>'VARCHAR(256) NOT NULL',	
						'status'=>'INTEGER NOT NULL',
						'flags'=>'INTEGER NOT NULL',
						'gperm_access_admin_panel'=>'INTEGER NOT NULL',
						'pperm_browse_crash_reports'=>'INTEGER NOT NULL',
						'pperm_browse_bugs'=>'INTEGER NOT NULL',
						'pperm_browse_debug_info'=>'INTEGER NOT NULL',
						'pperm_manage_crash_reports'=>'INTEGER NOT NULL',
						'pperm_manage_bugs'=>'INTEGER NOT NULL',
						'pperm_manage_debug_info'=>'INTEGER NOT NULL',
						'default_sidebar_tab'=>'VARCHAR(16) NOT NULL',
						'default_bug_status_filter'=>'VARCHAR(16) NOT NULL',
					)
				);
		
		// Insert standard user groups
		$this->insert('{{usergroup}}', 	
				array(					
					'name'=>'Admin',
					'description'=>'Administrators',	
					'status'=>UserGroup::STATUS_ACTIVE,
					'flags'=>UserGroup::FLAG_STANDARD_GROUP | 
				             UserGroup::FLAG_CANT_UPDATE | 
				             UserGroup::FLAG_CANT_UPDATE,
					'gperm_access_admin_panel'=>1,
					'pperm_browse_crash_reports'=>1,
					'pperm_browse_bugs'=>1,
					'pperm_browse_debug_info'=>1,
					'pperm_manage_crash_reports'=>1,
					'pperm_manage_bugs'=>1,
					'pperm_manage_debug_info'=>1,
					'default_sidebar_tab'=>'Digest',
					'default_bug_status_filter'=>'open',
				));
		
		$this->insert('{{usergroup}}', 	
				array(					
					'name'=>'Dev',
					'description'=>'Developers',	
					'status'=>UserGroup::STATUS_ACTIVE,
					'flags'=>UserGroup::FLAG_STANDARD_GROUP,
					'gperm_access_admin_panel'=>0,
					'pperm_browse_crash_reports'=>1,
					'pperm_browse_bugs'=>1,
					'pperm_browse_debug_info'=>1,
					'pperm_manage_crash_reports'=>1,
					'pperm_manage_bugs'=>1,
					'pperm_manage_debug_info'=>1,
					'default_sidebar_tab'=>'Digest',
					'default_bug_status_filter'=>'owned',
				));
		
		$this->insert('{{usergroup}}', 	
				array(					
					'name'=>'QA',
					'description'=>'Quality Assurance',	
					'status'=>UserGroup::STATUS_ACTIVE,
					'flags'=>UserGroup::FLAG_STANDARD_GROUP,
					'gperm_access_admin_panel'=>0,
					'pperm_browse_crash_reports'=>1,
					'pperm_browse_bugs'=>1,
					'pperm_browse_debug_info'=>0,
					'pperm_manage_crash_reports'=>1,
					'pperm_manage_bugs'=>1,
					'pperm_manage_debug_info'=>0,
					'default_sidebar_tab'=>'Digest',
					'default_bug_status_filter'=>'verify',
				));
			
		$this->insert('{{usergroup}}', 	
				array(					
					'name'=>'Guest',
					'description'=>'Limited Users',	
					'status'=>UserGroup::STATUS_ACTIVE,
					'flags'=>UserGroup::FLAG_STANDARD_GROUP,
					'gperm_access_admin_panel'=>0,
					'pperm_browse_crash_reports'=>1,
					'pperm_browse_bugs'=>1,
					'pperm_browse_debug_info'=>0,
					'pperm_manage_crash_reports'=>0,
					'pperm_manage_bugs'=>0,
					'pperm_manage_debug_info'=>0,
					'default_sidebar_tab'=>'Digest',
					'default_bug_status_filter'=>'open',
				));
		
		// Create {{user}} table
		$this->createTable('{{user}}',
				array(
					'id'=>'pk',
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'username'=>'VARCHAR(128) NOT NULL UNIQUE',
					'usergroup'=>'INTEGER NOT NULL',
					'password'=>'VARCHAR(128) NOT NULL',
					'salt'=>'VARCHAR(128) NOT NULL',
					'pwd_reset_token'=>'VARCHAR(128)',
					'status'=>'INTEGER NOT NULL',
					'flags'=>'INTEGER NOT NULL',
					'email'=>'VARCHAR(128) NOT NULL',
				)
			);
		
		// Insert root user
		$this->insert('{{user}}', 	
				array(					
					'username'=>'root',
					'usergroup'=>1,
					'password'=>'7dcfe0dd0af7bf680e0ae5e410ac02ee',
					'salt'=>'e8wqopb0guk8ryhbargqzfahmqaz5td8',
					'pwd_reset_token'=>null,
					'status'=>User::STATUS_ACTIVE,
					'flags'=>User::FLAG_STANDARD_USER|User::FLAG_PASSWORD_RESETTED,
					'email'=>'test@localhost',	
				));
		
		// Create {{appversion}} table
		$this->createTable('{{appversion}}',
				array(
					'id'=>'pk',
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'project_id'=>'INTEGER NOT NULL',
					'version'=>'VARCHAR(32) NOT NULL',
					'CONSTRAINT uc_Version UNIQUE (project_id, version)'
				)
			);
		
		// Create {{project}} table
		$this->createTable('{{project}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'id'=>'pk',
					'name'=>'VARCHAR(32) NOT NULL',
					'description'=>'VARCHAR(256)',
					'status'=>'INTEGER NOT NULL',
					'crash_reports_per_group_quota'=>'INTEGER NOT NULL',
					'crash_report_files_disc_quota'=>'INTEGER NOT NULL',
					'bug_attachment_files_disc_quota'=>'INTEGER NOT NULL',
					'debug_info_files_disc_quota'=>'INTEGER NOT NULL'
				)
			);
				
		// Create {{user_project_access}} table
		$this->createTable('{{user_project_access}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'id'=>'pk',
					'user_id'=>'INTEGER NOT NULL',
					'project_id'=>'INTEGER NOT NULL',
					'usergroup_id'=>'INTEGER NOT NULL',
					//CONSTRAINT uc_UserID UNIQUE (user_id, project_id)
				)
			);
		
		// Create {{crashreport}} table
		$this->createTable('{{crashreport}}',
				array(
					//'id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',		
					'id'=>'pk',
					'srcfilename'=>'VARCHAR(512) NOT NULL',
					'filesize'=>'INTEGER NOT NULL',
					'date_created'=>'INTEGER',
					'received'=>'INTEGER NOT NULL',
					'status'=>'INTEGER NOT NULL',	
					'ipaddress'=>'VARCHAR(32)',
					'md5'=>'VARCHAR(32) NOT NULL',
					'groupid'=>'INTEGER NOT NULL',
					'crashguid'=>'VARCHAR(36)',
					'project_id'=>'INTEGER NOT NULL',
					'appversion_id'=>'INTEGER NOT NULL',
					'emailfrom'=>'VARCHAR(32)',
					'description'=>'TEXT',
					'crashrptver'=>'VARCHAR(16)',
					'exception_type'=>'VARCHAR(64)',
					'exception_code'=>'BIGINT',
					'exception_thread_id'=>'INTEGER',
					'exceptionmodule'=>'VARCHAR(512)',
					'exceptionmodulebase'=>'BIGINT',
					'exceptionaddress'=>'BIGINT',
					'exe_image'=>'VARCHAR(1024)',	
					'os_name_reg'=>'VARCHAR(512)',
					'os_ver_mdmp'=>'VARCHAR(128)',
					'os_is_64bit'=>'INTEGER',
					'geo_location'=>'VARCHAR(16)',
					'product_type'=>'VARCHAR(128)',
					'cpu_architecture'=>'VARCHAR(64)',
					'cpu_count'=>'INTEGER',
					'gui_resource_count'=>'INTEGER',
					'open_handle_count'=>'INTEGER',
					'memory_usage_kbytes'=>'INTEGER'	
				)
			);
		
		// Create {{fileitem}} table
		$this->createTable('{{fileitem}}',
				array(					
					'id'=>'pk',
					'crashreport_id'=>'INTEGER NOT NULL',
					'filename'=>'VARCHAR(512) NOT NULL',					
					'description'=>'VARCHAR(512)'
				)
			);
		
		// Create {{customprop}} table
		$this->createTable('{{customprop}}',
				array(					
					'id'=>'pk',						
					'crashreport_id'=>'INTEGER NOT NULL',
					'name'=>'VARCHAR(128) NOT NULL',
					'value'=>'TEXT NOT NULL'
				)
			);
		
		// Create {{thread}} table
		$this->createTable('{{thread}}',
				array(					
					'id'=>'pk',
					'thread_id'=>'INTEGER NOT NULL',
					'crashreport_id'=>'INTEGER NOT NULL',
					'stack_trace_md5'=>'VARCHAR(32)'
				)
			);
		
		// Create {{stackframe}} table
		$this->createTable('{{stackframe}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',	
					'id'=>'pk',
					'thread_id'=>'INTEGER NOT NULL',
					'addr_pc'=>'BIGINT NOT NULL',
					'module_id'=>'INTEGER',
					'offs_in_module'=>'INTEGER',
					'symbol_name'=>'VARCHAR(2048)',
					'und_symbol_name'=>'VARCHAR(2048)',
					'offs_in_symbol'=>'INTEGER',
					'src_file_name'=>'VARCHAR(512)',
					'src_line'=>'INTEGER',
					'offs_in_line'=>'INTEGER'	

				)
			);
		
		// Create {{module}} table
		$this->createTable('{{module}}',
				array(					
					'id'=>'pk',
					'crashreport_id'=>'INTEGER NOT NULL',
					'name'=>'VARCHAR(512) NOT NULL',
					'sym_load_status'=>'INTEGER NOT NULL',
					'loaded_debug_info_id'=>'INTEGER',
					'file_version'=>'VARCHAR(32) NOT NULL',
					'timestamp'=>'INTEGER',
				)
			);
		
		// Create {{processingerror}} table
		$this->createTable('{{processingerror}}',
				array(					
					'id'=>'pk',
					'type'=>'INTEGER NOT NULL',
					'srcid'=>'INTEGER NOT NULL',
					'message'=>'TEXT'
				)
			);
		
		// Create {{crashgroup}} table
		$this->createTable('{{crashgroup}}',
				array(					
					'id'=>'pk',
					'created'=>'INTEGER NOT NULL',
					'status'=>'INTEGER NOT NULL',
					'project_id'=>'INTEGER NOT NULL',
					'appversion_id'=>'INTEGER NOT NULL',	
					'title'=>'VARCHAR(256) NOT NULL',
					'md5'=>'VARCHAR(32) NOT NULL'
				)
			);
		
		// Create {{bug}} table
		$this->createTable('{{bug}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'id'=>'pk',
					'date_created'=>'INTEGER NOT NULL',
					'date_last_modified'=>'INTEGER NOT NULL',
					'date_closed'=>'INTEGER',
					'project_id'=>'INTEGER NOT NULL',
					'appversion_id'=>'INTEGER NOT NULL',
					'status'=>'INTEGER NOT NULL',	
					'summary'=>'VARCHAR(256) NOT NULL',
					'description'=>'TEXT NOT NULL',
					'reported_by'=>'INTEGER NOT NULL',
					'assigned_to'=>'INTEGER',
					'priority'=>'INTEGER NOT NULL',
					'reproducability'=>'INTEGER NOT NULL',
					'merged_into'=>'INTEGER'
				)
			);
		
		// Create {{bug_change}} table
		$this->createTable('{{bug_change}}',
				array(					
					'id'=>'pk',
					'bug_id'=>'INTEGER NOT NULL',
					'timestamp'=>'INTEGER NOT NULL',	
					'user_id'=>'INTEGER NOT NULL',
					'flags'=>'INTEGER NOT NULL',
					'status_change_id'=>'INTEGER',
					'comment_id'=>'INTEGER'
				)
			);
		
		// Create {{bug_status_change}} table
		$this->createTable('{{bug_status_change}}',
				array(					
					'id'=>'pk',
					'status'=>'INTEGER',	
					'assigned_to'=>'INTEGER',
					'priority'=>'INTEGER',
					'reproducability'=>'INTEGER',
					'merged_into'=>'INTEGER'
				)
			);
		
		// Create {{bug_attachment}} table
		$this->createTable('{{bug_attachment}}',
				array(					
					'id'=>'pk',
					'bug_change_id'=>'INTEGER NOT NULL',
					'filename'=>'VARCHAR(512) NOT NULL',
					'filesize'=>'INTEGER NOT NULL',
					'md5'=>'VARCHAR(32) NOT NULL'
				)
			);
		
		// Create {{bug_comment}} table
		$this->createTable('{{bug_comment}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'id'=>'pk',
					'text'=>'TEXT'
				)
			);
		
		// Create {{bug_crashreport}} table
		$this->createTable('{{bug_crashreport}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'id'=>'pk',
					'bug_id'=>'INTEGER NOT NULL',
					'crashreport_id'=>'INTEGER NOT NULL',
					//CONSTRAINT uc_Mapping UNIQUE (bug_id, crashreport_id)
				)
			);
		
		// Create {{bug_crashgroup}} table
		$this->createTable('{{bug_crashgroup}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'id'=>'pk',
					'bug_id'=>'INTEGER NOT NULL',
					'crashgroup_id'=>'INTEGER NOT NULL',
					// CONSTRAINTuc_Mapping UNIQUE (bug_id, crashgroup_id)
				)
			);
		
		// Create {{debuginfo}} table
		$this->createTable('{{debuginfo}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',
					'id'=>'pk',
					'project_id'=>'INTEGER NOT NULL',
					'dateuploaded'=>'INTEGER NOT NULL',
					'status'=>'INTEGER NOT NULL',
					'filename'=>'VARCHAR(512) NOT NULL',
					'guid'=>'VARCHAR(48) NOT NULL',
					'md5'=>'VARCHAR(32) NOT NULL',
					'filesize'=>'INTEGER NOT NULL'
				)
			);
		
		// Create {{operation}} table
		$this->createTable('{{operation}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',	
					'id'=>'pk',
					'status'=>'INTEGER NOT NULL',
					'timestamp'=>'INTEGER NOT NULL',
					'optype'=>'INTEGER NOT NULL',
					'srcid'=>'INTEGER NOT NULL',
					'cmdid'=>'VARCHAR(32) NOT NULL',
					'operand1'=>'TEXT',	
					'operand2'=>'TEXT',
					'operand3'=>'TEXT'
				)
			);
		
		// Create {{mail_queue}} table
		$this->createTable('{{mail_queue}}',
				array(
					//'id'=>'INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT',	
					'id'=>'pk',
					'create_time'=>'INTEGER NOT NULL',
					'status'=>'INTEGER NOT NULL',
					'sent_time'=>'INTEGER',
					'recipient'=>'VARCHAR(1024) NOT NULL',
					'email_subject'=>'VARCHAR(256) NOT NULL',
					'email_headers'=>'VARCHAR(1024) NOT NULL',
					'email_body'=>'TEXT NOT NULL'					
				)
			);
		
		// Create {{AuthItem}} table
		$this->createTable('{{AuthItem}}',
				array(
					"name"=>"varchar(64) not null",
					"type"=>"integer not null",
					"description"=>"text",
					"bizrule"=>"text",
					"data"=>"text",
					$this->q('primary key ("name")')
				)
			);
		
		// Create {{AuthItemChild}} table
		$this->createTable('{{AuthItemChild}}',
				array(
				    "parent"=>'varchar(64) not null',
					"child"=>'varchar(64) not null',
					$this->q('primary key ("parent","child")'),
					$this->q('foreign key ("parent") references "{{AuthItem}}" ("name") on delete cascade on update cascade'),
					$this->q('foreign key ("child") references "{{AuthItem}}" ("name") on delete cascade on update cascade')
				)
			);
		
		// Create {{AuthAssignment}} table
		$this->createTable('{{AuthAssignment}}',
				array(
					"itemname"=>'varchar(64) not null',
					"userid"=>'varchar(64) not null',
					"bizrule"=>'text',
					"data"=>'text',
					$this->q('primary key ("itemname","userid")'),
					$this->q('foreign key ("itemname") references "{{AuthItem}}" ("name") on delete cascade on update cascade')
				)
			);
		
		return true;		
	}

	public function down()
	{
		echo "m120515_122132_initial does not support migration down.\n";
		return false;
	}

	/*
	// Use safeUp/safeDown to do migration with transaction
	public function safeUp()
	{
	}

	public function safeDown()
	{
	}
	*/
}

