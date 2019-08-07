<?php

/**
 * This is the model class for table "{{mail_queue}}".
 *
 * The followings are the available columns in table '{{mail_queue}}':
 * @property integer $id
 * @property integer $create_time
 * @property integer $status
 * @property integer $sent_time
 * @property string $recipient
 * @property string $email_subject
 * @property string $email_headers
 * @property string $email_body
 */
class MailQueue extends CActiveRecord
{
	const STATUS_PENDING   = 1; // The email is awaiting in the queue.
	const STATUS_SENT      = 2; // The email has been sent.
	const STATUS_FAILED    = 3; // The email send operation has failed.
	
    public $isSimpleSearch = true; // Simple search flag
    public $searchFilter; // Simple search query
    public $statusFilter; // Simple search status filter
    
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return MailQueue the static model class
	 */
	public static function model($className=__CLASS__)
	{
		return parent::model($className);
	}

	/**
	 * @return string the associated database table name
	 */
	public function tableName()
	{
		return '{{mail_queue}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('create_time, status, recipient', 'required'),
			array('create_time, status, sent_time', 'numerical', 'integerOnly'=>true),
			array('recipient', 'length', 'max'=>32),
			array('email_subject', 'length', 'max'=>256),
			array('email_headers', 'length', 'max'=>1024),
			array('email_body', 'length', 'max'=>2048),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, create_time, status, sent_time, recipient, email_subject, email_headers, email_body', 'safe', 'on'=>'search'),
		);
	}
	
	/**
	 * Adds an email to the queue.
	 * @param type $recipient Recipient's email address.
	 * @param type $emailSubject Email subject.
	 * @param type $emailText Email text.
	 * @param type $headers Email headers.
	 * @return boolean On success, returns true.
	 */
	public static function addMail($recipient, $emailSubject, $emailText)
	{	
		// Format email headers
		$emailFrom = "no-reply@".Yii::app()->request->serverName;
		$headers="From: {$emailFrom}\r\nReply-To: {$emailFrom}";		
		
		// Create new email item
		$queueItem = new MailQueue();
		$queueItem->recipient = $recipient;
		$queueItem->email_subject = $emailSubject;
		$queueItem->email_body = $emailText;
		$queueItem->email_headers = $headers;
		$queueItem->create_time = time();
		$queueItem->status = self::STATUS_PENDING;
		
		// Save an item to queue
		if(!$queueItem->save())
		{
			// Error
			$errors = $queueItem->getErrors();
			foreach($errors as $fieldName=>$fieldErrors)
			{
				foreach($fieldErrors as $error)
					Yii::log('Error adding email to queue: '.$error, 'error');				
			}
			return false;
		}
		
		// OK
		return true;
	}
	
	
	/**
	 * Sends pending mail messages.
	 * @return void
	 */
	public static function sendMail()
	{
		// Prepare search criteria
		$criteria = new CDbCriteria;
		$criteria->condition = 'status='.self::STATUS_PENDING;
		$criteria->order = 'create_time ASC';
		$criteria->limit = 5;		
		
		// Look for pending email items
		$pendingItems = MailQueue::model()->findAll($criteria);
		
		// Walk through pending items
		foreach($pendingItems as $email)
		{
			// Send email
			$result = @mail($email->recipient, 
					        $email->email_subject, 
							$email->email_body, 
							$email->email_headers);		
			if($result==true)
			{
				// Succeeded
				$email->status = self::STATUS_SENT;				
				$email->sent_time = time();
			}
			else
			{
				// Failed
				$email->status = self::STATUS_FAILED;
				Yii::log('Error sending email ', 'info');	
			}
			
			// Save changes to DB
			$email->save();
		}
	}
    
    /**
     * Returns the list of statuses.
     * @return array list of statuses. 
     */
    public function getStatusList() {
        $list[self::STATUS_PENDING] = 'Pending';
        $list[self::STATUS_SENT] = 'Sent';
        $list[self::STATUS_FAILED] = 'Failed';

        return $list;
    }

    /**
     * Returns status string.
     */
    public function getStatusStr() {
        $statuses = $this->getStatusList();
        return $statuses[$this->status];
    }
    
    /**
     * Retrieves a list of models based on the current search/filter conditions.
     * @return CActiveDataProvider the data provider that can return the models based on the search/filter conditions.
     */
    public function search() 
    {
        $criteria = new CDbCriteria;

        if($this->isSimpleSearch)
        {
            if(isset($this->searchFilter))
            {
                $criteria->compare('recipient', $this->searchFilter, true, 'OR');
                $criteria->compare('email_subject', $this->searchFilter, true, 'OR');                
            }
            
            if(isset($this->searchFilter) && $this->statusFilter!=-1)
                $criteria->compare('status', $this->statusFilter, false, 'AND');                            
        }
        else
        {
            $criteria->compare('id', $this->id);
            $criteria->compare('recipient', $this->recipient, true);            
            $criteria->compare('status', $this->status, true);
        }
        
        $dataProvider = new CActiveDataProvider($this, array(
                    'criteria' => $criteria,
                    'sort'=>array(
                        'defaultOrder'=>'create_time DESC'
                    ),
                    'pagination'=>array(
                        'pageSize'=>30
                    )
                ));
        
        return $dataProvider; 
    } 
}