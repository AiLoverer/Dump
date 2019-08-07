<?php

class m130125_051239_add_module_guidnage_field extends CDbMigration
{
    /*
	public function up()
	{
        
	}

	public function down()
	{
		echo "m130125_051239_add_module_guidnage_field does not support migration down.\n";
		return false;
	}*/
	
	// Use safeUp/safeDown to do migration with transaction
	public function safeUp()
	{
        // Add 'matching_pdb_guid' column for {{module}} table
        $this->addColumn(
                '{{module}}', 
                'matching_pdb_guid', 
                'VARCHAR(48)'
                );
        
        return true;
	}

	public function safeDown()
	{
    	echo "m130125_051239_add_module_guidnage_field does not support migration down.\n";
		return false;
	}

}