<div class="view">

	<b><?php echo CHtml::encode($data->getAttributeLabel('id')); ?>:</b>
	<?php echo CHtml::link(CHtml::encode($data->id), array('view', 'id'=>$data->id)); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('crashrptver')); ?>:</b>
	<?php echo CHtml::encode($data->crashrptver); ?>
	<br />
	
	<b><?php echo CHtml::encode($data->getAttributeLabel('received')); ?>:</b>
	<?php echo CHtml::encode(strftime('%x %X', $data->received)); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('md5')); ?>:</b>
	<?php echo CHtml::encode($data->md5); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('crashguid')); ?>:</b>
	<?php echo CHtml::encode($data->crashguid); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('appname')); ?>:</b>
	<?php echo CHtml::encode($data->appname); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('appversion')); ?>:</b>
	<?php echo CHtml::encode($data->appversion); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('emailfrom')); ?>:</b>
	<?php echo CHtml::encode($data->emailfrom); ?>
	<br />
	
	<b><?php echo CHtml::encode($data->getAttributeLabel('description')); ?>:</b>
	<?php echo CHtml::encode($data->description); ?>
	<br />
	
	<b><?php echo CHtml::encode($data->getAttributeLabel('exceptionmodule')); ?>:</b>
	<?php echo CHtml::encode($data->exceptionmodule); ?>
	<br />
	
	<b><?php echo CHtml::encode($data->getAttributeLabel('exceptionmodulebase')); ?>:</b>
	<?php echo CHtml::encode($data->exceptionmodulebase); ?>
	<br />
	
	<b><?php echo CHtml::encode($data->getAttributeLabel('exceptionaddress')); ?>:</b>
	<?php echo CHtml::encode($data->exceptionaddress); ?>
	<br />

</div>