<?php
include(drupal_get_path('module','module_sys').'/includes/module_sys.functions.inc');
/**
 * Module install admin.
 */
function module_sys_admin_form($form, $form_state) { 
  $form['module_contrib_directory'] = array(
    '#type' => 'textfield',
    '#title' => t('Module contrib directory'),
    '#default_value' => variable_get('module_contrib_directory', 'sites/all/modules/contrib'),
    '#size' => 60,
    '#required' => true,
  );
  $form['module_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter the machine name of the module to install'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['install'] = array(
    '#type' => 'submit',
    '#value' => t('Install'),
  );
  $form['#submit'] = array(
    'module_sys_admin_form_submit',
  );
  return system_settings_form($form);
}


/*
 * Module sys admin form submit function
 */
function module_sys_admin_form_submit($form,$form_state){
	$values = $form_state['values'];
	if($values['op'] == 'Install'){
		if(empty($values['module_name'])){
			form_set_error('module_name',t('This field is required'));
		}else{
			$modulename = $values['module_name'];
			if(module_exists($modulename)){
				drupal_set_message($modulename.' is already installed','status');
			}else{
				installModule($modulename);
			}
		}
	} 
	
}