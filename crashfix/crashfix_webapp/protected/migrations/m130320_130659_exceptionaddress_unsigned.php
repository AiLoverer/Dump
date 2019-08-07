<?php

class m130320_130659_exceptionaddress_unsigned extends CDbMigration
{
    /**
	public function up()
	{        
	}

	public function down()
	{
		echo "m130320_130659_exceptionaddress_unsigned does not support migration down.\n";
		return false;
	}*/

	
	// Use safeUp/safeDown to do migration with transaction
	public function safeUp()
	{
        // Make exceptionaddress unsigned bigint
        $this->alterColumn(
                '{{crashreport}}',
                'exceptionaddress',
                'BIGINT UNSIGNED'
                );
        
        // Make exceptionmodulebase unsigned bigint
        $this->alterColumn(
                '{{crashreport}}',
                'exceptionmodulebase',
                'BIGINT UNSIGNED'
                );
        
        // Make exception_code unsigned bigint
        $this->alterColumn(
                '{{crashreport}}',
                'exception_code',
                'BIGINT UNSIGNED'
                );
        
	}

	public function safeDown()
	{
        echo "m130320_130659_exceptionaddress_unsigned does not support migration down.\n";
		return false;
	}
	
}