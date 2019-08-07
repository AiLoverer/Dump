<?php

/**
 * This is the model class for table "{{bug_attachment}}".
 *
 * The followings are the available columns in table '{{bug_attachment}}':
 * @property integer $id
 * @property string $filename
 */
class BugAttachment extends CActiveRecord
{
	public $fileAttachment; // File attachment
	public $ignoreFileAttachmentErrors = false; // Used for tests
	
	/**
	 * Returns the static model of the specified AR class.
	 * @param string $className active record class name.
	 * @return BugAttachment the static model class
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
		return '{{bug_attachment}}';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		return array(						
			array('filename', 'length', 'max'=>512),
			// The following rule is used by search().
			// Please remove those attributes that should not be searched.
			array('id, filename', 'safe', 'on'=>'search'),
		);
	}

	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
			'bugChange'=>array(self::BELONGS_TO, 'BugChange', 'bug_change_id'),
		);
	}
	
	/**
	 * This method is executed before saving AR to database.
	 * @return boolean true on success.
	 */
	public function beforeSave()
	{
		if(!parent::beforeSave())
			return false;
		
		// Move uploaded file to its persistent location.
		$this->saveFileAttachment();
		
		return true;
	}
	
	/**
	 * This method saves uploaded file to its persistent location. 
	 * @throws Exception
	 */
	public function saveFileAttachment()
	{
		// Set filename attribute
		$this->filename = basename($this->fileAttachment->getName());

		// Set filesize attribute
		$this->filesize = $this->fileAttachment->getSize();

		// Get bug model this attachment belongs to.
		$bugChange = BugChange::model()->findByPk($this->bug_change_id);		
		$bug = Bug::model()->findByPk($bugChange->bug_id);
		
		// Check project quota
        if($bug->project->bug_attachment_files_disc_quota>0)
        {
            $totalFileSize = 0;
            $percentOfDiskQuota = 0;
            $bug->project->getBugAttachmentCount($totalFileSize, 
                    $percentOfDiskQuota);

            if($bug->project->bug_attachment_files_disc_quota*1024*1024-
                    $totalFileSize-$this->filesize<0)
            {
                $this->addError('fileAttachment', 'Bug attachment quota for this project has exceeded.');
                throw new Exception('Bug attachment quota constrained.');
            }
        }

		// Calc md5 hash and save it as model attribute
		$this->md5 = md5_file($this->fileAttachment->getTempName());

		// Create the directory where we will place the uploaded file
		$subDir1 = substr($this->md5, 0, 3);
		$subDir2 = substr($this->md5, 3, 3);
		$dirName = Yii::app()->getBasePath()."/data/bugAttachments/".$subDir1."/".$subDir2;
		if(!@is_dir($dirName))
		{
			if(False==@mkdir($dirName, 0777, TRUE))
			{
				$error = error_get_last();
				$this->addError("fileAttachment", "Couldn't make directory for file attachment.");
				throw new Exception('Could not make directory for file attachment.');
			}
		}

		// Move uploaded file to an appropriate directory and delete temp file
		$fileName = $dirName."/".$this->md5;				
		if(!$this->fileAttachment->saveAs($fileName, true) && 
		   !$this->ignoreFileAttachmentErrors)
		{
			$this->addError("fileAttachment", "Couldn't save file attachment"); 
			throw new Exception('Could not save file attachment');
		}													
	}
	
	/**
	 * This method returns the absolute local path to this attachment file.
	 * @return string file path on success, or false on failure.	 
	 */
	public function getLocalFilePath()
	{
		// Validate MD5 hash
		if(!isset($this->md5) || strlen($this->md5)!=32)
			return false;
						
		// Determine path to local file
		$subDir1 = substr($this->md5, 0, 3);
		$subDir2 = substr($this->md5, 3, 3);				
		$dirName = Yii::app()->getBasePath()."/data/bugAttachments/".$subDir1."/".$subDir2;	
		$fileName = $dirName."/".$this->md5;
		return $fileName;
	}
	
	/**
	 * This method dumps the content of attachment file to stdout or to a file.
	 * This method is used when downloading the bug attachment file.
	 * @param string $file Filename. Can be null (in this case dumped to stdout).
	 * @throws CHttpException
	 * @return void
	 */
	public function dumpFileAttachmentContent($file = null)
	{
		// Try to open the output file
		$fout=false;
		if($file!=null)
		{
			$fout = fopen($file, 'w');
			if($fout==false)
				throw new CHttpException(403, 'Page does not exists.');
		}
		
		// Determine path to local file for download
		$fileName = $this->getLocalFilePath();
		if($fileName==false)
			throw new CHttpException(403, 'Page does not exists');
		
		// Try to open file
		if ($fd = @fopen ($fileName, "r")) 
		{			
			// Determine file size.
			$fsize = filesize($fileName);			
			
			// Write HTTP headers
			header("Content-type: application/octet-stream");
			header("Content-Disposition: filename=\"".$this->filename."\"");
    		header("Content-length: $fsize");
			header("Cache-control: private"); //use this to open files directly
			
			// Write file content
			while(!feof($fd)) 
			{
				$buffer = fread($fd, 2048);
				
				if($file==null)
					echo $buffer;
				else
					fwrite($fout, $buffer);
			}
		}	
		
		// Close file
		if($file!=null)
			fclose($fout);
	}
	
	/**
	 * This method is executed before AR is deleted from database.
	 * @return boolean True on success.
	 */
	protected function beforeDelete()
	{
		if(!parent::beforeDelete())	
			return false;
						
		// Get local file path to attachment file
		$fileName = $this->getLocalFilePath();
		
		// Get parent directory names
		$dirName = dirname($fileName);
		$outerDirName = dirname($dirName);
		
		// Remove attachment file
		@unlink($fileName);
		
		// Try to delete file owning directory (if it is empty)
		@rmdir($dirName);
		
		// Try to delete outer directory (if it is empty)
		@rmdir($outerDirName);
		
		// OK
		return true;
		
	}
}