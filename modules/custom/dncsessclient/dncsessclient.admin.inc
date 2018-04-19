<?php

/**
 * @file
 * Handling administration action in the system
 *
 * All configuration processing will be handled
 * by this module
 */

/**
 * Configure session client parameters
 *
 * @param array $form
 *   variable arry of form
 * @param array $form_state
 *   variable array of form_state
 *
 * @return array
 *   form array that will rendered by drupal_get_form
 */
function dncsessclient_admin_params(array $form = array(), array $form_state = array()) {
  $form_state['storage']['module'] = 'dncsessclient';
  return dncctypevars_content_type_preparation($form, $form_state);
}

function dncsessclient_admin_params_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  if (empty($form_state['values']['scheme'])) {
    dncctypevars_content_type_preparation_submit($form, $form_state);
    return;
  }

  $form_state['redirect'] = current_path();
  drupal_set_message(t('configurations have been successfully saved :)'));
}

function dncsessclient_show_testing_service_request() {
  $base_url = 'http://localhost:8081/dgsignage/testing-api';

    $ctarget = 'public://testing-cookie.txt';
    $ctarget = drupal_realpath($ctarget);
    $cookie = file_get_contents($ctarget);
    file_get_contents($ctarget, $cookie);
    // Now recycle the login cookie we received in the first request
    $options['headers']['Cookie'] = $cookie;

    // Get info about a user
    $data = array();
    $options['data'] = http_build_query($data, '', '&');
    $options['method'] = 'POST';
//    $response = drupal_http_request($base_url . '/user/1', $options);
    $response = drupal_http_request($base_url . '/user/token', $options);
    unset ($ctarget, $cookie);
    return $response;


  $data = array(
    'username' => 'root',
    'password' => 'root',
  );
  $data = drupal_json_encode($data);
  $options = array(
    'headers' => array(
      'Content-Type' => 'application/json',
    ),
    'method' => 'POST',
    'data' => $data
  );

  $response = drupal_http_request($base_url . '/user/login', $options);
  $data = json_decode($response->data);

  // Check if login was successful
  if ($response->code == 200) {
    $ctarget = 'public://testing-cookie.txt';
    $ctarget = drupal_realpath($ctarget);
    file_put_contents($ctarget, $data->session_name . '=' . $data->sessid);
    $cookie = file_get_contents($ctarget);
    // Now recycle the login cookie we received in the first request
    $options['headers']['Cookie'] = $cookie;

    // Get info about a user
    $data = array();
    $options['data'] = http_build_query($data, '', '&');
    $options['method'] = 'GET';
    $response = drupal_http_request($base_url . '/user/1', $options);
    unset ($ctarget);
  }
  return array($data, $response);
}

function dncsessclient_manager(array $form = array(), array $form_state = array()) {
  $form['items'] = array(
    '#type' => 'tableselect',
    '#title' => t('Remote Credentials'),
    '#weight' => 10,
    '#empty' => t('No data available yet :('),
    '#options' => [],
    '#header' => [],
  );
  $form['items']['#header']['nid']['data'] = t('Remote ID');
  $form['items']['#header']['remoteurl']['data'] = t('Remote Base URL');
  $form['items']['#header']['username']['data'] = t('Remote Username');
  $form['items']['#header']['edit']['data'] = t('Edit');
  $form['items']['#header']['action']['data'] = t('Action');

  $data = dncsessclient_get_credentials();
  // echo '<pre>'; print_r($data); echo '</pre>'; exit;
  if (empty($data)) {
    unset ($data);
    return $form;
  }

  foreach ($data as $keys => $values) {
    if (empty($values['detail']['baseurl']) || empty($values['detail']['username'])) {
      continue;
    }
    $form['items']['#options'][$values['nid']]['nid']['data'] = $values['nid'];
    $form['items']['#options'][$values['nid']]['remoteurl']['data'] = $values['detail']['baseurl'];
    $form['items']['#options'][$values['nid']]['username']['data'] = $values['detail']['username'];
    $form['items']['#options'][$values['nid']]['edit']['data'] = l(t('edit'), 'admin/structure/dncsystem/dncsessclient-update/' . $values['nid']);
    $login = empty($values['cookie']) ? l(t('login'), 'dncsessclient-login/' . $values['nid']) : l(t('logout'), 'dncsessclient-logout/' . $values['nid']);
    $form['items']['#options'][$values['nid']]['action']['data'] = $login;
    unset ($login);
  }
  unset ($data);

  $form['submit'] = array(
    '#weight' => 100,
    '#type' => 'submit',
    '#value' => t('delete choosed'),
  );
  return $form;
}

function dncsessclient_manager_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  if (!empty($form_state['input']['items'])) {
    module_load_include('inc', 'dncsystem', 'dncsystem.content');
    foreach ($form_state['input']['items'] as $keys => $values) {
      if (!empty($values)) {
        $data = dncsystem_content_get_data('dncsessclient', 'dncsessclientcreds', $keys);
        if (!empty($data[$keys])) {
          $node = node_load($keys);
          $node->status = NODE_NOT_PUBLISHED;
          node_save($node);
          unset ($node);
        }
        unset ($data);
      }
    }
  }
  drupal_set_message(t('Delete choosed items are done :)'));
}

function dncsessclient_manager_add(array $form = array(), array $form_state = array()) {
  return dncsessclient_manager_update($form, $form_state);
}

function dncsessclient_manager_edit(array $form = array(), array $form_state = array(), $nid) {
  $form_state['storage']['nid'] = $nid;
  return dncsessclient_manager_update($form, $form_state);
}

function dncsessclient_manager_update(&$form, &$form_state) {
  if (!empty($form_state['storage']['nid'])) {
    $data = dncsessclient_get_credentials($form_state['storage']['nid']);
    if (empty($data)) {
      unset ($data);
      drupal_set_message(t('No remote credential found by the ID :('), 'error', FALSE);
      return $form;
    }
    $default = $data;
    unset ($data);
    $form['nid'] = array(
      '#type' => 'value',
      '#value' => $form_state['storage']['nid'],
    );
  }

  // force altering breadcrumbs
  $breadcrumb = array();
  $breadcrumb[] = l(t('home'), '<front>');
  $breadcrumb[] = l(t('listing'), 'admin/structure/dncsystem/dncsessclient');
  drupal_set_breadcrumb($breadcrumb);
  unset ($breadcrumb);

  $form['credential'] = array(
    '#type' => 'fieldset',
    '#weight' => 10,
    '#title' => t('Remote Informations'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['credential']['baseurl'] = array(
    '#type' => 'textfield',
    '#weight' => 10,
    '#title' => t('Remote Base URL'),
    '#size' => 60,
    '#default_value' => empty($default['detail']['baseurl']) ? NULL : $default['detail']['baseurl'],
    '#required' => TRUE,
  );
  $form['credential']['service_path'] = array(
    '#type' => 'textfield',
    '#weight' => 20,
    '#title' => t('Remote Service Path'),
    '#size' => 40,
    '#default_value' => empty($default['detail']['service_path']) ? NULL : $default['detail']['service_path'],
    '#required' => TRUE,
  );
  $form['credential']['token_method_path'] = array(
    '#type' => 'textfield',
    '#weight' => 30,
    '#title' => t('Remote Token Method Path'),
    '#size' => 40,
    '#default_value' => empty($default['detail']['token_method_path']) ? NULL : $default['detail']['token_method_path'],
  );
  $form['credential']['username'] = array(
    '#type' => 'textfield',
    '#weight' => 40,
    '#title' => t('Remote User Name'),
    '#size' => 40,
    '#default_value' => empty($default['detail']['username']) ? NULL : $default['detail']['username'],
    '#required' => TRUE,
  );
  $form['credential']['password'] = array(
    '#type' => 'textfield',
    '#weight' => 50,
    '#title' => t('Remote Password'),
    '#size' => 40,
    '#default_value' => empty($default['detail']['password']) ? NULL : $default['detail']['password'],
    '#required' => TRUE,
  );
  $form['credential']['connect_path'] = array(
    '#type' => 'textfield',
    '#weight' => 60,
    '#title' => t('Remote Connect Path'),
    '#size' => 40,
    '#default_value' => empty($default['detail']['connect_path']) ? NULL : $default['detail']['connect_path'],
    '#required' => TRUE,
  );
  $form['credential']['login_path'] = array(
    '#type' => 'textfield',
    '#weight' => 70,
    '#title' => t('Remote Login Path'),
    '#size' => 40,
    '#default_value' => empty($default['detail']['login_path']) ? NULL : $default['detail']['login_path'],
    '#required' => TRUE,
  );
  $form['credential']['logout_path'] = array(
    '#type' => 'textfield',
    '#weight' => 80,
    '#title' => t('Remote Logout Path'),
    '#size' => 40,
    '#default_value' => empty($default['detail']['logout_path']) ? NULL : $default['detail']['logout_path'],
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 100,
    '#value' => t('save'),
  );
  return $form;
}

function dncsessclient_manager_update_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = empty($_GET['destination']) ? 'admin/structure/dncsystem/dncsessclient' : $_GET['destination'];
  unset ($_GET['destination']);
  if (empty($form_state['input']['baseurl']) || empty($form_state['input']['username'])) {
    return;
  }

  // remove ended slash
  $form_state['input']['baseurl'] = preg_replace('/\/$/', '', $form_state['input']['baseurl']);
  $form_state['input']['service_path'] = preg_replace('/^\//', '', $form_state['input']['service_path']);
  $form_state['input']['service_path'] = preg_replace('/\/$/', '', $form_state['input']['service_path']);
  $form_state['input']['connect_path'] = preg_replace('/^\//', '', $form_state['input']['connect_path']);
  $form_state['input']['connect_path'] = preg_replace('/\/$/', '', $form_state['input']['connect_path']);
  $form_state['input']['login_path'] = preg_replace('/^\//', '', $form_state['input']['login_path']);
  $form_state['input']['login_path'] = preg_replace('/\/$/', '', $form_state['input']['login_path']);
  $form_state['input']['logout_path'] = preg_replace('/^\//', '', $form_state['input']['logout_path']);
  $form_state['input']['logout_path'] = preg_replace('/\/$/', '', $form_state['input']['logout_path']);
  $form_state['input']['token_method_path'] = preg_replace('/^\//', '', $form_state['input']['token_method_path']);
  $form_state['input']['token_method_path'] = preg_replace('/\/$/', '', $form_state['input']['token_method_path']);

  $default = [];
  if (!empty($form_state['values']['nid'])) {
    $default = dncsessclient_get_credentials($form_state['values']['nid']);
  }
  else {
    $tsearch = md5($form_state['input']['baseurl'] . $form_state['input']['username']);
    $data = dncsessclient_get_credentials(NULL, $tsearch);
    if (empty($data)) {
      $default['title'] = $tsearch;
    }
    else {
      $default = $data;
    }
    unset ($tsearch, $data);
  }
  $default['detail']['baseurl'] = $form_state['input']['baseurl'];
  $default['detail']['service_path'] = $form_state['input']['service_path'];
  $default['detail']['token_method_path'] = $form_state['input']['token_method_path'];
  $default['detail']['username'] = $form_state['input']['username'];
  $default['detail']['password'] = $form_state['input']['password'];
  $default['detail']['connect_path'] = $form_state['input']['connect_path'];
  $default['detail']['login_path'] = $form_state['input']['login_path'];
  $default['detail']['logout_path'] = $form_state['input']['logout_path'];

  $default['detail'] = json_encode($default['detail']);
  $default['detail'] = encrypt($default['detail']);
  $default['detail'] = base64_encode($default['detail']);

  module_load_include('inc', 'dncsystem', 'dncsystem.content');
  dncsystem_content_update_data('dncsessclient', 'dncsessclientcreds', $default);
  unset ($default);
  drupal_set_message(t('Remote credential saved successfully :)'));
}

function dncsessclient_manager_add_submit(&$form, &$form_state) {
  dncsessclient_manager_update_submit($form, $form_state);
}

function dncsessclient_manager_edit_submit(&$form, &$form_state) {
  dncsessclient_manager_update_submit($form, $form_state);
}
