<?php

class m121114_141005_user_cur_project_field extends CDbMigration
{
    /*
	public function up()
	{
        
	}

	public function down()
	{
		echo "m121114_141005_user_cur_project_field does not support migration down.\n";
		return false;
	}
    */     

	// Use safeUp/safeDown to do migration with transaction
	public function safeUp()
	{
        // Add 'cur_project_id' column for {{user}} table
        $this->addColumn(
                '{{user}}', 
                'cur_project_id', 
                'INTEGER'
                );
        
        // Add 'cur_appversion_id' column for {{user}} table
        $this->addColumn(
                '{{user}}', 
                'cur_appversion_id', 
                'INTEGER'
                );
	}

	public function safeDown()
	{
        echo "m121114_141005_user_cur_project_field does not support migration down.\n";
		return false;
	}
	
}