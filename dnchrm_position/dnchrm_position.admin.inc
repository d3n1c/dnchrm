<?php

/**
 * @file
 * Handling administration action in the system
 * 
 * All configuration processing will be handled
 * by this module
 */

/**
 * Configure position data of HRM system parameters
 * 
 * @param array $form
 *   variable arry of form
 * @param array $form_state
 *   variable array of form_state
 * 
 * @return array
 *   form array that will rendered by drupal_get_form
 */
function dnchrm_position_admin_params(array $form = array(), array $form_state = array()) {
  $info = dnchrm_position_content_type_schema_vars(FALSE, NULL, TRUE);
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
  
  $form['dnchrm_position_level_max'] = array(
    '#type' => 'textfield',
    '#size' => 5,
    '#title' => t('Level Maximum'),
    '#description' => t('Maximum amount of available organization structure\'s levels'),
    '#required' => TRUE,
    '#default_value' => variable_get('dnchrm_position_level_max', 3),
  );
  
  return system_settings_form($form);
}

/**
 * Building form to manage position references
 * 
 * @param array $form
 * @param array $form_state
 * @param string $filter
 *   argument extra to limit operation in certain effective date
 * 
 * @return array
 *   form array
 */
function dnchrm_position_listing(array $form = array(), array $form_state = array(), $effective_date = NULL) {
  $scheme = dnchrm_position_content_type_schema_vars(TRUE, 'positions');
  if (empty($scheme['content_type'])) {
    unset ($scheme);
    drupal_set_message(t('Wrong scheme parameters :('), 'error', FALSE);
    return $form;
  }
  $nodetype = node_type_get_type($scheme['content_type']);
  
  if (empty($form_state['storage']['effective_date'])) {
    if (empty($_GET['effective_date']) && empty($effective_date)) {
      $form['effective_date'] = array();
      foreach ($scheme['fields'] as $keys => $values) {
        if (in_array($keys, array('effective_date'))) {
          if ($values == 'title') {
            $form[$keys]['#title'] = $nodetype->title_label;
            $form[$keys]['#type'] = 'textfield';
            $form[$keys]['#required'] = TRUE;
            $form[$keys]['#size'] = 40;
          }
          else {
            $instance = field_info_instance('node', $values, $scheme['content_type']);
            $form[$keys]['#title'] = $instance['label'];
            $form[$keys]['#required'] = $instance['required'];
            $form[$keys]['#description'] = $instance['description'];
            $finfo = field_info_field($values);
            if ($instance['widget']['type'] == 'date_popup') {
              $form[$keys]['#type'] = 'date_popup';
              foreach ($instance['widget']['settings'] as $ky => $vl) {
                if (!empty($vl)) {
                  $ky = '#date_' . preg_replace('/^input_/', '', $ky);
                  if ($ky == '#date_format') {
                    $vl = dncsystem_granularity_to_date_format($finfo['settings']['granularity']);
                  }
                  $form[$keys][$ky] = $vl;
                }
              }
            }
            elseif($instance['widget']['type'] == 'options_onoff') {
              $form[$keys]['#type'] = 'checkbox';
              $form[$keys]['#title'] = $instance['label'] . ' ?!';
            }
            elseif (preg_match('/^list/', $finfo['type'])) {
              $form[$keys]['#type'] = 'select';
              $form[$keys]['#options'] = $finfo['settings']['allowed_values'];
            }
            elseif ($finfo['type'] == 'entityreference') {
              $options = array();
              $conditions = array();
              foreach ($finfo['settings']['handler_settings']['target_bundles'] as $vl) {
                if (!empty($vl)) {
                  $conditions['type'][] = $vl;
                }
              }
              $dump = entity_load($finfo['settings']['target_type'], FALSE, array('type' => $conditions));
              unset ($conditions);
              if (!empty($dump)) {
                foreach ($dump as $node) {
                  $options[$node->nid] = $node->title;
                }
              }
              unset ($dump);
              $form[$keys]['#type'] = 'select';
              $form[$keys]['#options'] = $options;
              unset ($options);
            }
            else {
              $form[$keys]['#type'] = 'textfield';
              $form[$keys]['#size'] = empty($instance['widget']['settings']['size']) ? 40 : $instance['widget']['settings']['size'];
              $form[$keys]['#maxlength'] = empty($finfo['settings']['max_length']) ? 255 : $finfo['settings']['max_length'];
            }
            unset ($finfo, $instance);
          }
        }
      }
      $form['setdate'] = array(
        '#type' => 'submit',
        '#value' => t('set'),
        '#submit' => array('dnchrm_position_references_set_date'),
        '#weight' => 100,
      );
      return $form;
    }
    else {
      if (empty($effective_date)) {
        if (empty($_GET['effective_date'])) {
          drupal_set_message(t('No effective date info available :('), 'error', FALSE);
          return $form;
        }
        $effective_date = $_GET['effective_date'];
      }
      $checkdate = empty($effective_date) ? FALSE : dncsystem_check_invalid_date($effective_date);
      if (empty($effective_date) || empty($checkdate)) {
        unset ($effective_date, $checkdate);
        drupal_set_message(t('No effective date info available :('), 'error', FALSE);
        return $form;
      }
      unset ($checkdate);
      $form_state['storage']['effective_date'] = $effective_date;
      unset ($effective_date);
    }
  }
  
  return dnchrm_position_references_form($form, $form_state);
}

/**
 * Execution action of types of rank reference management
 * 
 * @param array $form
 * @param array $form_state
 */
function dnchrm_position_listing_submit(&$form, &$form_state) {
  dnchrm_position_references_form_submit($form, $form_state);
  $effective_date = $form_state['values']['effective_date'];
  $form_state['redirect'] = array(current_path(), array(
    'query' => array(
      'effective_date' => $effective_date
    )
  ));
  unset ($effective_date);
  drupal_set_message(t('Positions have been saved :)'));
}

/**
 * Submit action of reference effective data choosing
 * 
 * @param array $form
 * @param array $form_state
 */
function dnchrm_position_references_set_date(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['rebuild'] = TRUE;
  $scheme = dnchrm_position_content_type_schema_vars(TRUE, 'positions');
  foreach ($form_state['values'] as $keys => $values) {
    if (!empty($values) && !empty($scheme['fields'][$keys])) {
      $form_state['storage'][$keys] = $values;
    }
  }
}

/**
 * Produce popup form to manage position
 * 
 * @param string $js
 */
function dnchrm_position_manage_position_popup($js = NULL, $effective_date) {
  // degrade half-assed to no js
  if(!$js) {
    return drupal_get_form('dnchrm_position_references_form', $effective_date);
  }
  
  // adelante!
  ctools_include('modal');
  ctools_include('ajax');
  $form_state['title'] = t('Manage Position');
  $form_state['ajax'] = TRUE;
  $form_state['storage']['effective_date'] = $effective_date;
  $output = ctools_modal_form_wrapper('dnchrm_position_references_form', $form_state);
  if (!empty($form_state['executed'])) {
    $output = array();
    $output[] = ctools_modal_command_dismiss(t('Success!'));
    $output[] = ctools_ajax_command_reload();
  }
  print ajax_render($output);
}