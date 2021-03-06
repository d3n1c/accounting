<?php

/**
 * @file
 * Handling administration action in the system
 *
 * All configuration processing will be handled
 * by this module
 */

/**
 * Configure heap of assets parameters
 *
 * @param array $form
 *   variable array of form
 * @param array $form_state
 *   variable array of form_state
 *
 * @return array
 *   form array that will rendered by drupal_get_form
 */
function dncstorage_admin_params(array $form = array(), array $form_state = array()) {
  $default = variable_get('dncstorage_configs', array());
  $form['dncstorage'] = array(
    '#type' => 'fieldset',
    '#title' => t('Storage Configuration Parameters'),
    '#weight' => 10,
    '#collapsible' => TRUE,
    '#collapsed' => empty($default) ? FALSE : TRUE,
  );
  $form['dncstorage']['dncstorage_configs[main_path]'] = array(
    '#type' => 'textfield',
    '#weight' => 10,
    '#title' => t('Main Path') . ' *',
    '#description' => t('Path where all assets are stored, Usually an absolute path outside the document root'),
    '#size' => 40,
    '#default_value' => empty($default['main_path']) ? 'accountings' : $default['main_path'],
  );
  $form['dncstorage']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('save'),
    '#weight' => 500,
  );
  return $form;
}

function dncstorage_admin_params_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);

  $form_state['redirect'] = current_path();
  if (!empty($form_state['input']['dncstorage_configs'])) {
    $mainpath = $form_state['input']['dncstorage_configs']['main_path'];
    $mainpath = (!preg_match('/^\//', $mainpath) ? file_default_scheme() . '://' : NULL) . $mainpath;
    $mainpath = drupal_realpath($mainpath);
    $form_state['input']['dncstorage_configs']['main_path'] = $mainpath;
    unset ($mainpath);
    variable_set('dncstorage_configs', $form_state['input']['dncstorage_configs']);
  }
  drupal_set_message(t('configurations have been successfully saved :)'));
}
