<?php

/**
 * @file
 * Install contribute module.
 */

include drupal_get_path('module', 'install_contrib_module') . '/includes/install_contrib_module.functions.inc';

/**
 * Module install admin.
 */
function install_contrib_module_admin_form($form, $form_state) {
  $form['module_contrib_directory'] = array(
    '#type' => 'textfield',
    '#title' => t('Module contrib directory'),
    '#default_value' => variable_get('module_contrib_directory', 'sites/all/modules/contrib'),
    '#size' => 60,
    '#required' => TRUE,
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
    '_install_contrib_module_admin_form_submit',
  );
  return system_settings_form($form);
}

/**
 * Module sys admin form submit function.
 */
function _install_contrib_module_admin_form_submit($form, $form_state) {
  $values = $form_state['values'];
  if ($values['op'] == 'Installer') {
    if (empty($values['module_name'])) {
      form_set_error('module_name', t('This field is required'));
    }
    else {
      $modulename = $values['module_name'];
      if (module_exists($modulename)) {
        drupal_set_message($modulename . ' is already installed', 'status');
      }
      else {
        try {
          installModule($modulename);
        }
        catch (Exception $e) {
          drupal_get_messages();
          drupal_set_message(t('An error occurred and processing did not complete.'), 'error');
        }
      }
    }
  }
}
