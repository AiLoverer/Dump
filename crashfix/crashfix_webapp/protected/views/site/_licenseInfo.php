<div class="div_license">

<?php if($licenseInfo!=False): ?>

<h5>Product Info:</h5>	

<ul class="daemon-status-list">	
	<li class="daemon-status-list-item"><span class="list-item-label">Product Name:</span>CrashFix</li>
	<li class="daemon-status-list-item"><span class="list-item-label">Product Version:</span> <?php echo CHtml::encode($licenseInfo['ProductVersion'])?></li>
	<li class="daemon-status-list-item">
		<span class="list-item-label">Edition:</span> 
		<?php 
			echo CHtml::encode($licenseInfo['Edition']);
			if($licenseInfo['Evaluation']==1)
			{
				echo ' (Evaluation version)';
			}
		?>
	</li>	
	<li class="daemon-status-list-item"><span class="list-item-label">Date Created:</span> <?php echo CHtml::encode($licenseInfo['DateCreated'])?></li>	
	<?php if($licenseInfo['Evaluation']==1): ?>
		<li class="daemon-status-list-item">
			<span class="list-item-label">Expires in:</span> 
			<?php echo CHtml::encode($licenseInfo['DaysExpiresFromNow'])?> day(s)			
		</li>	
	<?php endif; ?>
</ul>		
	
<h5 class="prepend-top">Customer Info:</h5>	

<ul class="">	
	<li class="daemon-status-list-item"><span class="list-item-label">Name:</span>&nbsp;<?php echo CHtml::encode($licenseInfo['Name'])?></li>
	<li class="daemon-status-list-item"><span class="list-item-label">Surname:</span>&nbsp;<?php echo CHtml::encode($licenseInfo['Surname'])?></li>	
</ul>		

<?php else: ?>

<div class="error">
<i>Error retrieving license information, because the daemon is inactive or not responding.</i>
</div>

<?php endif; ?>

</div>
