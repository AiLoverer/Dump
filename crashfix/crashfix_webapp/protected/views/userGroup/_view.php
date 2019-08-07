<div class="view">

	<b><?php echo CHtml::encode($data->getAttributeLabel('id')); ?>:</b>
	<?php echo CHtml::link(CHtml::encode($data->id), array('view', 'id'=>$data->id)); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('name')); ?>:</b>
	<?php echo CHtml::encode($data->name); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_browse_crash_reports')); ?>:</b>
	<?php echo CHtml::encode($data->can_browse_crash_reports); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_browse_bugs')); ?>:</b>
	<?php echo CHtml::encode($data->can_browse_bugs); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_browse_debug_info')); ?>:</b>
	<?php echo CHtml::encode($data->can_browse_debug_info); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_browse_sys_info')); ?>:</b>
	<?php echo CHtml::encode($data->can_browse_sys_info); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_manage_crash_reports')); ?>:</b>
	<?php echo CHtml::encode($data->can_manage_crash_reports); ?>
	<br />

	<?php /*
	<b><?php echo CHtml::encode($data->getAttributeLabel('can_manage_bugs')); ?>:</b>
	<?php echo CHtml::encode($data->can_manage_bugs); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_manage_debug_info')); ?>:</b>
	<?php echo CHtml::encode($data->can_manage_debug_info); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_manage_users')); ?>:</b>
	<?php echo CHtml::encode($data->can_manage_users); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('can_manage_quotas')); ?>:</b>
	<?php echo CHtml::encode($data->can_manage_quotas); ?>
	<br />

	*/ ?>

</div>