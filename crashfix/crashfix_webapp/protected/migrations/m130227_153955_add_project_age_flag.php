<?php

class m130227_153955_add_project_age_flag extends CDbMigration
{
    /*
	public function up()
	{
	}

	public function down()
	{
		echo "m130227_153955_add_project_age_flag does not support migration down.\n";
		return false;
	}
    */
	
	// Use safeUp/safeDown to do migration with transaction
	public function safeUp()
	{
        // Add 'require_exact_build_age' column for {{project}} table
        $this->addColumn(
                '{{project}}', 
                'require_exact_build_age', 
                'INTEGER NOT NULL DEFAULT 1'
                );
        
        return true;
	}

	public function safeDown()
	{
        echo "m130227_153955_add_project_age_flag does not support migration down.\n";
		return false;
	}
	
}