<?php

/**
 *   This test case contains tests for MailQueue class.
 */
class MailQueueTest extends CDbTestCase
{	
	/**
	 *   The fixtures used by this test case.
	 */
	public $fixtures=array(
        'mail'=>':test_mail_queue',
    );
		
    public function testAddMail()
	{		
		// Some cheating
		$_SERVER["SERVER_NAME"] = '127.0.0.1';
		
		$emailSubject = 'Test';
		$emailText = 'Text of the mail';
		$recipient = 'test@localhost';
		
		// Add email to mail queue
		$added = MailQueue::addMail($recipient, $emailSubject, $emailText);
		// Ensure added
		$this->assertTrue($added);
	}	
	
	public function testSendMail()
	{	
		// Some cheating
		$_SERVER["SERVER_NAME"] = '127.0.0.1';
		
		// Send pending mail
		MailQueue::sendMail();
		
		// Ensure there are no pending emails
		$emails = MailQueue::model()->findAll('status='.MailQueue::STATUS_PENDING);		
		$this->assertTrue(count($emails)==0);
	}
    
    public function testGetStatusStr()
    {
        $model = MailQueue::model()->findByPk(1);
        $this->assertTrue($model!=null);
        
        $this->assertTrue($model->statusStr=='Pending');
    }
    
    public function testSimpleSearch() 
    {
        // Create a new model containing search filter info
        $model = new MailQueue('search');
        $model->isSimpleSearch = true;
        $model->searchFilter = 'Test Email Subject'; 
        
        // Search - this will return a data provider
        $dataProvider = $model->search();
        // Expect one payment found
        $this->assertTrue(count($dataProvider->data) == 1);        
    }
    
    public function testAdvancedSearch() 
    {
        // Create a new model containing search filter info
        $model = new MailQueue('search');
        $model->isSimpleSearch = false;
        $model->recipient = 'test@localhost';
        
        // Search - this will return a data provider
        $dataProvider = $model->search();
        // Expect one payment found
        $this->assertTrue(count($dataProvider->data) == 1);        
    }
}