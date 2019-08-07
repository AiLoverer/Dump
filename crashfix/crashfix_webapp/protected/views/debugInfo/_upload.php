<?php 

	if($model->hasErrors()) 
	{
		// Output the first error message.
		$errors = $model->getErrors();
		foreach ($errors as $key=>$val)
		{		
			foreach ($val as $attr=>$error)
			{
				echo "450 ".$error;
				break;
			}
			break;
		}
			
	}
	else
	{
		// Output success message.
		echo '200 Success.';
	}
	
?>