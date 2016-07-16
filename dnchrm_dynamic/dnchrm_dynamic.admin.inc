<?php

/**
 * @file
 * Handling administration action in the system
 * 
 * All configuration processing will be handled
 * by this module
 */

/**
 * Configure dynamic personality datas of HRM system parameters
 * 
 * @param array $form
 *   variable arry of form
 * @param array $form_state
 *   variable array of form_state
 * 
 * @return array
 *   form array that will rendered by drupal_get_form
 */
function dnchrm_dynamic_admin_params(array $form = array(), array $form_state = array()) {
  $info = dnchrm_dynamic_content_type_schema_vars(FALSE, NULL, TRUE);
  $form['content types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Types'),
    '#description' => t('Main content type that be used in this system'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 0,
  );
  $form['content types']['scheme'] = array(
    '#markup' => $info,
  );
  unset ($info);
  
  $form['dnchrm_dynamic_active_context'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Active Contexts'),
    '#description' => t('Where context will be activated ?!'),
    '#options' => array(
      'personal_characteristics' => t('Personal Characteristics'),
      'employee_religions' => t('Employee Religions'),
      'social_identities' => t('Social Identities'),
      'formal_addresses' => t('Formal Addresses'),
      'mastery_of_languages' => t('Mastery of Language'),
    ),
    '#default_value' => variable_get('dnchrm_dynamic_active_context', array()),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('save'),
    '#weight' => 200,
  );
  
  return $form;
}

function dnchrm_dynamic_admin_params_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  variable_set('dnchrm_dynamic_active_context', $form_state['input']['dnchrm_dynamic_active_context']);
  drupal_set_message(t('Configuration saved :)'));
  menu_rebuild();
}

