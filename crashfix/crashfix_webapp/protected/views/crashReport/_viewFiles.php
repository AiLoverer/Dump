
<div class="span-18 last">
	<div class="box">
		<ul class="daemon-status-list">			
			<li class="daemon-status-list-item loud"><div class="list-item-label">File Name:</div><?php echo CHtml::encode($model->srcfilename); ?></li>
			<li class="daemon-status-list-item loud"><div class="list-item-label">File Size:</div><?php echo CHtml::encode(MiscHelpers::fileSizeToStr($model->filesize));?></li>
			<li class="daemon-status-list-item loud"><div class="list-item-label">MD5 Hash:</div><?php echo CHtml::encode($model->md5);?></li>
			<li class="daemon-status-list-item"><?php echo CHtml::link('Download Entire ZIP Archive', array('crashReport/download', 'id'=>$model->id)); ?></li>			
		</ul>
	</div>
</div>

<div class="span-18 last">

<div class="quiet">You can also download individual files contained in this crash report archive:</div>

<!-- Grid view -->
<?php $this->widget('zii.widgets.grid.CGridView', array(
      'dataProvider'=>$model->searchFileItems(),
	  'selectableRows'=>null,
      'columns'=>array(
		  array(            
              'name' => 'filename',
			  'type' => 'raw',
			  'value' => 'CHtml::link($data->filename, array(\'crashReport/extractFile\', \'name\'=>$data->filename, \'rpt\'=>$data->crashreport_id))',	  			  			  
          ),
		  'description'
      ),
 )); 
  
 ?>

</div>