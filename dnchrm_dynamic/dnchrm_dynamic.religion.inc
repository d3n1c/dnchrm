<?php

/* 
 * Copyright (C) 2016 denic wibowo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * Module to handle religions of human resource management
 * 
 * Handling religions data to handle management of human resources
 */

/**
 * Produce popup form to add new personal characteristic type
 * 
 * @param string $js
 */
function dnchrm_dynamic_add_new_religion_popup($js = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['employee_religions'])) {
    unset ($activecontext);
    return NULL;
  }
  unset ($activecontext);
  // degrade half-assed to no js
  if(!$js) {
    return drupal_get_form('dnchrm_register_reference');
  }
  
  // adelante!
  ctools_include('modal');
  ctools_include('ajax');
  $form_state['title'] = t('Religions');
  $form_state['ajax'] = TRUE;
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['context'] = 'religions';
  $output = ctools_modal_form_wrapper('dnchrm_register_reference', $form_state);
  if (!empty($form_state['executed'])) {
    $output = array();
    $output[] = ctools_modal_command_dismiss(t('Success!'));
    $output[] = ctools_ajax_command_reload();
  }
  print ajax_render($output);
}

/**
 * Retrieving all religion references
 * 
 * @return array
 */
function dnchrm_dynamic_get_all_religions() {
  $scheme = dnchrm_dynamic_content_type_schema_vars(TRUE, 'religions');
  $result = entity_load('node', FALSE, array(
    'status' => NODE_PUBLISHED,
    'type' => $scheme['content_type'],
  ));
  if (empty($result)) {
    unset ($result, $scheme);
    return;
  }
  $return = array();
  foreach ($result as $node) {
    $return[$node->nid] = dncsystem_retrieve_node_data($scheme, $node, 'title');
  }
  unset ($scheme, $node, $result);
  return $return;
}

/**
 * Get present active religion of the person
 * 
 * @param int $person
 *   NID of the person
 * 
 * @return array
 *   data array information of the person's present religion
 */
function dnchrm_dynamic_get_religion_by_person($person) {
  $data = dnchrm_get_data_by_person($person, 'dnchrm_dynamic', 'employee_religions', NULL, 'full');
  if (empty($data)) {
    unset ($data);
    return;
  }
  $stampoftime = NULL;
  $return = array();
  foreach ($data as $values) {
    if ($values['date_changed'] > $stampoftime) {
      $stampoftime = $values['date_changed'];
      $return = $values;
    }
  }
  unset ($stampoftime, $data);
  return $return;
}

