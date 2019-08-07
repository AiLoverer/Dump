<?php 

	$dataProvider = $model->searchFileItems();
	$count=0;
	foreach($dataProvider->data as $fileItem)
	{	
		if(!preg_match('/video.ogg/', $fileItem->filename))
			continue;
		$count++;
?>

<div class="img">
	<a target="_blank" href="<?php echo $this->createUrl('viewVideo', array('name'=>$fileItem->filename, 'rpt'=>$model->id)); ?>">
		<video width="320" height="240" controls autoplay>
            <source src="<?php echo $this->createUrl('viewVideo', array('name'=>$fileItem->filename, 'rpt'=>$model->id)); ?>" type="video/ogg">
            Your browser does not support video
        </video>
	</a>
	<div class="div.desc"><?php echo CHtml::link($fileItem->filename, array('crashReport/viewVideo', 'name'=>$fileItem->filename, 'rpt'=>$model->id)); ?></div>
</div>

<?php
	}	
	
	if($count==0)
		echo '<i>There are no videos available.</i>';
?>