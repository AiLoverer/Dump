<?php

/**
 * Used for batch import of crash report files and/or symbol files from the import
 * directory. 
 */
class BatchImporter
{
    const IMPORT_CRASH_REPORTS = 1;
    const IMPORT_DEBUG_INFO    = 2;
    
    private $_importedCrashReportsCount = 0;
    private $_importedDebugInfoCount = 0;
    
    /**
     * Performs batch import of crash reports and symbol files. 
     * @param $importDir Directory name where to import from.
     * @param $countOfCrashReports On output, receives count of crash report files imported.
     * @param $countOfDebugInfo On output, receives count of debug info files imported. 
     * @return boolean true on success; otherwise false.
     * 
     * Import directory must be organized in the following way:
     * 
     * /path/to/import/dir
     *     /crashReports
     *       /project1
     *          /version1
     *              /report1.zip
     *              /report2.zip
     *          /version2
     *              /report3.zip
     *              /report4.zip
     *       /projectName2
     *          /version1
     *              /report1.zip
     *    /debugInfo
     *       /project1
     *          /version1
     *             /symbol1.pdb
     *             /symbol2.pdb
     *       /project2
     *          /version1
     *             /symbol3.pdb
     */
    public function importFiles($dirName, &$countOfCrashReports, &$countOfDebugInfo)
    {
        //echo 'Importing files from dir: '.$dirName.'\n';
        
        // Validate input param
        if(!is_dir($dirName))
        {
            Yii::log('BatchImporter: not a directory: '.$dirName, 'error');
            return false;
        }
        
        $importLockFile = $dirName.'/importlock';
        if(file_exists($importLockFile))
        {
            Yii::log('BatchImporter: found import lock file '.$importLockFile.'; import not allowed.');
            return false;
        }
               
        // Reset counters.
        $this->_importedCrashReportsCount = 0;
        $this->_importedDebugInfoCount = 0;
        
        // Import from crashReports subdirectory
        $this->importFilesOf($dirName.'/crashReports', 
                self::IMPORT_CRASH_REPORTS);
        
        // Import from debugInfo subdirectory
        $this->importFilesOf($dirName.'/debugInfo', 
                self::IMPORT_DEBUG_INFO);
        
        $countOfCrashReports = $this->_importedCrashReportsCount;
        $countOfDebugInfo = $this->_importedDebugInfoCount;
        
        // Done
        return true;
    }    
    
    /**
     * Imports files of certain type.
     * @param string $dirName Import directory.
     * @param string $type Type of files to import.     
     * @return boolean true on success.
     */
    private function importFilesOf($dirName, $type)
    {
        //echo 'Importing files of certain type from dir: '.$dirName.'\n';
        
        // Validate input param
        if(!is_dir($dirName))
        {
            Yii::log('BatchImporter: not a directory: '.$dirName, 'error');
            return false;
        }
        
        // Get file list in the directory
        $fileList = scandir($dirName);
        if($fileList==false)
        {
            Yii::log('Directory name is invalid: '.$dirName, 'error');
            return false;
        }
        
        // Walk through files
        foreach($fileList as $index=>$file)
        {
            if($file!='.' && $file!='..' && is_dir($dirName.'/'.$file))
            {
                $subDir = $dirName.'/'.$file;                
                $this->importProjectFiles($subDir, $file, $type);                
            }
        }
        
        // Done
        return true;
    } 
        
    /**
     * Import files for certain project.
     * @param string $dirName Directory where to look for files.
     * @param string $projName Name of the project.
     * @return true on success; otherwise false.
     */
    private function importProjectFiles($dirName, $projName, $type)
    {
        //echo 'Importing project files from dir: '.$dirName.'\n';
        
        // Look for such a project name
        $project = Project::model()->find('name=:name', 
            			array(':name'=>$projName));
		if($project===null)
		{
            Yii::log('Such a project name not found: '.$projName, 'error');
            return false;
        }
        
        // Get file list in the directory
        $fileList = scandir($dirName);
        if($fileList==false)
        {
            Yii::log('Directory name is invalid: '.$dirName, 'error');
            return false;
        }
        
        // Walk through files
        foreach($fileList as $index=>$file)
        {
            if($file!='.' && $file!='..' && is_dir($dirName.'/'.$file))
            {
                $appver = AppVersion::createIfNotExists($file, $project->id);
                
                $subDir = $dirName.'/'.$file;
                if($type==self::IMPORT_CRASH_REPORTS)
                    $this->importCrashReports($subDir, $project->id, $appver->id);
                else
                    $this->importDebugInfo($subDir, $project->id, $appver->id);
            }
        }
        
        // Done
        return true;
    }
        
    /**
     * Imports crash report files for certain project version.
     * @param string $dirName  Directory name.
     * @param string $projectId Project ID.
     * @param string $projVer  Project version.
     * @return integer count of crash report imported; or -1 on error.
     */
    private function importCrashReports($dirName, $projectId, $projVerId)
    {
        //echo 'Importing crash report files from dir: '.$dirName.'\n';
        
        // Get file list in the directory
        $fileList = scandir($dirName);
        if($fileList==false)
        {
            Yii::log('Directory name is invalid: '.$dirName, 'error');
            return -1; // Error
        }
                
        // Walk through files
        foreach($fileList as $index=>$file)
        {
            // Get abs path
            $path = $dirName.'/'.$file;  
                       
            // Strip file parts
            $path_parts = pathinfo($path);
        
            if($file!='.' && $file!='..' && 
               is_file($path) && strtolower($path_parts['extension'])=='zip')        
            {
                //echo 'Importing crash report file: '.$path.'\n';
                
                // Create new AR record
                $crashReport = new CrashReport();
                $crashReport->project_id = $projectId;
                $crashReport->appversion = $projVerId;
                $crashReport->fileAttachment = new CUploadedFile(
												$file, 
												$path,
												'application/zip',
												filesize($path),
												''
												);	
                // The following is to copy attachment file correctly.
                $crashReport->fileAttachmentIsUploaded = false; 
                
                // Save changes to database
                if(!$crashReport->save())
                {
                    Yii::log('Could not import crash report file:'.$path, 'error');
                    $errors = $crashReport->getErrors();
                    //var_dump($errors);
                }
                else
                {                
                    // Increment counter
                    $this->_importedCrashReportsCount++;
                }
            }
        }
        
        // Done
        return $this->_importedCrashReportsCount;
    }
    
    /**
     * Imports debug info files for certain project version.
     * @param string $dirName  Directory name.
     * @param string $projectId Project ID.
     * @param string $projVer  Project version.
     * @return integer count of debug info files imported; or -1 on error.
     */
    private function importDebugInfo($dirName, $projectId, $projVerId)
    {
        //echo 'Importing debug info files from dir: '.$dirName.'\n';
        
        // Get file list in the directory
        $fileList = scandir($dirName);
        if($fileList==false)
        {
            Yii::log('Directory name is invalid: '.$dirName, 'error');
            return -1;
        }
        
        // Walk through files
        foreach($fileList as $file)
        {
            // Get abs path
            $path = $dirName.'/'.$file;            
            // Strip file parts
            $path_parts = pathinfo($path);
        
            if($file!='.' && $file!='..' && 
               is_file($path) && strtolower($path_parts['extension'])=='pdb')        
            {
                //echo 'Importing debug info file: '.$path.'\n';
                
                // Create new AR record
                $debugInfo = new DebugInfo();
                $debugInfo->project_id = $projectId;       
                $debugInfo->guid = 'tmp_'.MiscHelpers::GUID();
                $debugInfo->fileAttachment = new CUploadedFile(
												$file, 
												$path,
												'application/zip',
												filesize($path),
												''
												);	
                // The following is to copy attachment file correctly.
                $debugInfo->fileAttachmentIsUploaded = false; 
                
                // Save changes to database
                if(!$debugInfo->save())
                {
                    Yii::log('Could not import debug info file:'.$path, 'error');                    
                }
                else
                {                
                    $this->_importedDebugInfoCount++;
                }
            }
        }
        
        // Done
        return $this->_importedDebugInfoCount;
    }
}

?>
