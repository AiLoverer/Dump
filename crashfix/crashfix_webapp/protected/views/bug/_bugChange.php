<div class="bug-change">
	
	<div class="bug-change-header">
		Change #<?php echo $changeNum ?> 
		by <?php echo CHtml::encode($model->user->username) ?> 
		on <?php echo date('j F Y, G:i', $model->timestamp) ?>
	</div>
	
	<?php 
		if($model->comment!=null)
		{				
			echo '<div class="bug-change-comment">'.CHtml::encode(str_replace("<br>", "\n", $model->comment->text)).'</div>';  				
		}		

		if($model->statuschange)
		{
			echo '<div class="bug-change-status"><ul>';

			if(isset($model->statuschange->status))
				echo '<li>Status: '.Lookup::item('BugStatus', $model->statuschange->status);
			if(isset($model->statuschange->merged_into))
				echo '<li>Merged into bug: '.CHtml::link($model->statuschange->merged_into, array('bug/view', 'id'=>$model->statuschange->merged_into));
			if(isset($model->statuschange->priority))
				echo '<li>Priority: '.Lookup::item('BugPriority', $model->statuschange->priority);
			if(isset($model->statuschange->reproducability))
				echo '<li>Reproducibility: '.Lookup::item('BugReproducability', $model->statuschange->reproducability);
			
            if(isset($model->statuschange->assigned_to))				
            {
                if($model->statuschange->assigned_to>=0)
                    echo '<li>Owner: '.(isset($model->statuschange->owner)?CHtml::encode($model->statuschange->owner->username):'user was deleted');
                else
                    echo '<li>Owner: nobody';
            }            
                
            echo '</ul></div>';
		}

		foreach($model->attachments as $attachment)
		{				
			echo '<div class="bug-change-attach">'.CHtml::image(Yii::app()->baseUrl.'/images/file.png').
				'File attached: '.						
					CHtml::link(CHtml::encode($attachment->filename), 
							array('bug/downloadAttachment', 'id'=>$attachment->id)).
				' ('.MiscHelpers::fileSizeToStr($attachment->filesize).')'.
				'</div>';
		}
	?>
		
	
</div>
