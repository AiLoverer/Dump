<?php $this->beginContent('//layouts/main'); ?>
<div class="span-32">
	
	<div class="span-4 colborder">		
		<div id="sidebar">						
			<?php 	$this->widget('MainMenuPortlet', 
					array('activeItem'=>isset($this->sidebarActiveItem)?$this->sidebarActiveItem:''));  
			?>
		</div>
    </div>
	
	<div class="span-18 last">
		<div id="adminmenu">
			<?php 
				if(isset($this->adminMenuItem)) 
				{
					$this->widget('AdminMenuPortlet', 
						array('activeItem'=>$this->adminMenuItem));  			
				}		
			?>
		</div>
	</div>
	
	<div class="span-26 last">
	<?php if(isset($this->breadcrumbs)):?>
		<?php $this->widget('zii.widgets.CBreadcrumbs', array(
			'links'=>$this->breadcrumbs			
		)); ?><!-- breadcrumbs -->
	<?php endif?>
	</div>
	
    <div class="span-26 last">
		<div id="content">
			<?php echo $content; ?>
		</div><!-- content -->
    </div>

</div>	
<?php $this->endContent(); ?>