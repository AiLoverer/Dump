<?php 

	$dataProvider = $model->searchFileItems();
	$count=0;
	foreach($dataProvider->data as $fileItem)
	{	
		if(!preg_match('/screenshot[0-9]{1,3}.[jpg|png]/', $fileItem->filename))
			continue;
		$count++;
?>

<div class="img">
	<a target="_blank" href="<?php echo $this->createUrl('viewScreenshot', array('name'=>$fileItem->filename, 'rpt'=>$model->id)); ?>">
		<img src="<?php echo $this->createUrl('viewScreenshotThumbnail', array('name'=>$fileItem->filename, 'rpt'=>$model->id)); ?>" 
			 alt="<?php echo CHtml::encode($fileItem->filename); ?>" width="220" height="190" />
	</a>
	<div class="div.desc"><?php echo CHtml::link($fileItem->filename, array('crashReport/viewScreenshot', 'name'=>$fileItem->filename, 'rpt'=>$model->id)); ?></div>
</div>

<?php
	}	
	
	if($count==0)
		echo '<i>There are no screenshots available.</i>';
?>