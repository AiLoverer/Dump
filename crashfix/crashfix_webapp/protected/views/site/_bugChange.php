<div class="bug-change">
	
	<div class="bug-change-header">
		<?php 
			echo CHtml::link('Bug #'.$model->bug_id, array('bug/view', 'id'=>$model->bug_id)); 
			if(($model->flags&BugChange::FLAG_INITIAL_CHANGE)!=0)
				echo ' has been opened ';
			else
				echo ' has been changed ';
		?> 
		by <?php echo CHtml::encode($model->user->username) ?> 
		on <?php echo date('j F Y, G:i', $model->timestamp) ?>
	</div>
	
	<?php 
	
		if(($model->flags&BugChange::FLAG_INITIAL_CHANGE)!=0)			
		{				
			echo '<div class="bug-change-comment">'.CHtml::encode(MiscHelpers::addEllipsis($model->bug->summary,150)).'</div>';  				
		}	
		
		if($model->comment!=null)
		{				
			echo '<div class="bug-change-comment">'.CHtml::encode(MiscHelpers::addEllipsis($model->comment->text,150)).'</div>';  				
		}		

		if($model->statuschange)
		{
			echo '<div class="bug-change-status bug-change-status-digest"><ul>';

			if(isset($model->statuschange->status))
				echo '<li>Status: '.Lookup::item('BugStatus', $model->statuschange->status);
			if(isset($model->statuschange->merged_into))
				echo '<li>Merged into bug: '.CHtml::link($model->statuschange->merged_into, array('bug/view', 'id'=>$model->statuschange->merged_into));
			if(isset($model->statuschange->priority))
				echo '<li>Priority: '.Lookup::item('BugPriority', $model->statuschange->priority);
			if(isset($model->statuschange->reproducability))
				echo '<li>Repro: '.Lookup::item('BugReproducability', $model->statuschange->reproducability);
			if(isset($model->statuschange->assigned_to) && isset($model->statuschange->owner))				
				echo '<li>Owner: '.CHtml::encode($model->statuschange->owner->username);
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
