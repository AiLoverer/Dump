<?php

class m121110_163852_bug_assigned_to_required extends CDbMigration
{
	/*public function up()
	{
        
	}

	public function down()
	{
		echo "m121110_163852_bug_assigned_to_required does not support migration down.\n";
		return false;
	}*/

	
	// Use safeUp/safeDown to do migration with transaction
	public function safeUp()
	{
        // Update 'assigned_to' and make it -1 where it was null
        /*$this->update(
                '{{bug}}',
                array('assigned_to'=>-1),
                'assigned_to=NULL');*/
        
        $this->execute('UPDATE {{bug}} SET assigned_to=-1 WHERE assigned_to IS NULL');
        
        // Make 'assigned_to' attribute of {bug} table required
        $this->alterColumn(
                '{{bug}}', 
                'assigned_to', 
                'INTEGER NOT NULL DEFAULT -1'
                );
	}

	public function safeDown()
	{
        echo "m121110_163852_bug_assigned_to_required does not support migration down.\n";
	}
	
}