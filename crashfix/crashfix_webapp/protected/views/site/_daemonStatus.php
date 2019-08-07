<div class="<?php echo $daemonRetCode==0?'flash-success':'flash-error';?>">
<ul class="daemon-status-list">
	<?php 
		foreach($list as $msg)
		{
			$msg=trim($msg);
			if(strlen($msg)==0)
				continue;
			echo '<li class="daemon-status-list-item">'.CHtml::encode($msg).'</li>';
		}		
	?>
</ul>		
</div>
