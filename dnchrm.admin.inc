<?php

/**
 * @file
 * Handling administration action in the system
 * 
 * All configuration processing will be handled
 * by this module
 */

/**
 * Configure Core of HRM system parameters
 * 
 * @param array $form
 *   variable arry of form
 * @param array $form_state
 *   variable array of form_state
 * 
 * @return array
 *   form array that will rendered by drupal_get_form
 */
function dnchrm_admin_params(array $form = array(), array $form_state = array()) {
  $info = dnchrm_content_type_schema_vars(FALSE, NULL, TRUE);
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
  
  return $form;
}

