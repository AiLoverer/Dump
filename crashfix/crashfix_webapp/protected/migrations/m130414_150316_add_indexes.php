<?php

class m130414_150316_add_indexes extends CDbMigration
{
	public function up()
	{
        $this->createIndex(
                'crashreport_crashguid', 
                '{{crashreport}}', 
                'crashguid', 
                false);
        
        $this->createIndex(
                'crashreport_project', 
                '{{crashreport}}', 
                'project_id, appversion_id, groupid', 
                false);
        
        $this->createIndex(
                'crashreport_crashgroup', 
                '{{crashreport}}', 
                'groupid', 
                false);
                
        $this->createIndex(
                'fileitem_crashreport', 
                '{{fileitem}}', 
                'crashreport_id', 
                false);
        
        $this->createIndex(
                'customprop_crashreport', 
                '{{customprop}}', 
                'crashreport_id', 
                false);
        
        $this->createIndex(
                'thread_crashreport', 
                '{{thread}}', 
                'crashreport_id', 
                false);
        
        $this->createIndex(
                'stackframe_thread', 
                '{{stackframe}}', 
                'thread_id', 
                false);
        
        $this->createIndex(
                'module_crashreport', 
                '{{module}}', 
                'crashreport_id', 
                false);
        
        $this->createIndex(
                'processingerror_src', 
                '{{processingerror}}', 
                'srcid, type', 
                false);
        
        $this->createIndex(
                'crashgroup_project', 
                '{{crashgroup}}', 
                'project_id, appversion_id', 
                false);
                
        $this->createIndex(
                'debuginfo_project', 
                '{{debuginfo}}', 
                'project_id', 
                false);
        
	}

	public function down()
	{
		echo "m130414_150316_add_indexes does not support migration down.\n";
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