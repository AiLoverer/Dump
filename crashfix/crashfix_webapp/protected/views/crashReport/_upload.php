<?php 
	if($model->hasErrors()) 
	{
		$errors = $model->getErrors();
		foreach( $errors as $errMsg )
		{
			// Output only the first error
			echo '450 '.$errMsg[0];
			break;
		}
	}
	else
	{
		// Report success
		echo '200 Success.';
	}
?>