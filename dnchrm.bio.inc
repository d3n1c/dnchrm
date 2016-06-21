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
 * Handling all operation to manipulate bio data
 */

/**
 * Listing data bio of persons
 * 
 * @param array $form
 * @param array $form_state
 * @return string|array
 */
function dnchrm_bio_listing(array $form = array(), array $form_state = array()) {
//  module_load_include('inc', 'dncsystem', 'dncsystem.content');
//  $data = dncsystem_content_get_data('dnchrm', 'persons', NULL, NULL, 'default');
  $scheme = dnchrm_content_type_schema_vars(TRUE, 'persons');
  if (empty($scheme['content_type'])) {
    unset ($scheme);
    return;
  }
  if (!empty($form_state['storage']['items'])) {
    $description = '';
    return confirm_form(
            $form,
            t('Are you sure to delete these items ?!'),
            current_path(),
            $description,
            t('Yes delete all these items'),
            t('cancel')
          );
  }
  $table = array();
  $table['header'] = array(
    'num' => array(
      'data' => t('Num.'),
      'width' => '5%',
      'style' => 'text-align: right',
      'type' => 'constant',
    ),
  );
  $nodetype = node_type_get_type($scheme['content_type']);
  foreach ($scheme['fields'] as $keys => $values) {
    if ($values == 'title') {
      $table['header'][$keys] = array(
        'data' => $nodetype->title_label,
        'type' => 'property',
        'specifier' => $values,
        'sort' => 'ASC',
      );
    }
    else {
      $instance = field_info_instance('node', $values, $scheme['content_type']);
      $table['header'][$keys]['data'] = $instance['label'];
      $table['header'][$keys]['type'] = 'field';
      unset ($instance);
      $finfo = field_info_field($values);
      $table['header'][$keys]['specifier'] = array(
        'field' => $values,
        'column' => $finfo['type'] == 'entityrefrence' ? 'target_id' : 'value',
      );
      unset ($finfo);
    }
  }
  $limit = 10;
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node');
  $query->entityCondition('bundle', $scheme['content_type']);
  $query->propertyCondition('status', NODE_PUBLISHED);
  $query->addMetaData('account', user_load(1));
  $query->pager($limit);
  $query->tableSort($table['header']);
  $result = $query->execute();
  unset ($query);
  if (empty($result['node'])) {
    unset ($result);
    return $form;
  }
  $table['rows'] = array();
  $nodes = node_load_multiple(array_keys($result['node']));
  $page = empty($_REQUEST['page']) ? 0 : $_REQUEST['page'];
  $i = ($page * $limit) + 1;
  unset ($result, $page, $limit);
  foreach ($nodes as $node) {
    $rows = array();
    $data = dncsystem_retrieve_node_data($scheme, $node, NULL, 'default');
    $rows['num'] = array(
      'data' => $i . '.',
      'style' => 'text-align: right',
    );
    foreach ($scheme['fields'] as $key => $value) {
      $display = !empty($data['v_' . $key]['#markup']) ? $data['v_' . $key]['#markup'] : $data[$key];
      if ($key == 'title') {
        $display = l($display, 'dnchrm/bio/' . $node->nid);
      }
      $rows[$key] = array(
        'data' => $display,
      );
      unset ($display);
    }
    $table['rows'][$node->nid] = $rows;
    unset ($data, $rows);
    $i++;
  }
  unset ($scheme, $i);
  if (user_access('use dnc hrm')) {
    $form['items'] = array(
      '#type' => 'tableselect',
      '#header' => $table['header'],
      '#options' => $table['rows'],
      '#caption' => t('Persons'),
      '#weight' => 20,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete Choosed'),
      '#weight' => 200,
    );
  }
  else {
    $form['ctable'] = array(
      '#type' => 'item',
      '#markup' => theme('table', $table),
      '#weight' => 20,
    );
  }
  unset ($table);
  return $form;
}

/**
 * Implementing hook_submit for listing data bio form
 * 
 * @param array $form
 * @param array $form_state
 * @return type
 */
function dnchrm_bio_listing_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  $items = array();
  foreach ($form_state['values']['items'] as $values) {
    if (!empty($values)) {
      $items[] = $values;
    }
  }
  if (user_access('use dnc hrm')) {
    // action here
    if (empty($items)) {
      unset ($items);
      drupal_set_message(t('At least choose one item please :('), 'error', FALSE);
      return;
    }
    foreach ($items as $values) {
      dnchrm_delete_person_action($values);
    }
    drupal_set_message(t('All choosed persons have been deleted :)'));
  }
  unset ($items);
}

/**
 * Generating form to add person
 * 
 * @return array
 *   Display node add form
 */
function dnchrm_bio_add_person() {
  $scheme = dnchrm_content_type_schema_vars(TRUE, 'persons');
  $master = array(
    'scheme' => $scheme,
    'module' => 'dnchrm',
    'master' => 'persons',
    'destination' => 'dnchrm/bio',
  );
  unset ($scheme);
  $master = json_encode($master);
  $master = urlencode(base64_encode($master));
  $output = drupal_get_form('dncsystem_add_contents', $master);
  unset ($master);
  return drupal_render($output);
}    

/**
 * Generate listing table and update form
 * to manage person's extended informations
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * 
 * @return string|array
 *   Form array that rendered by drupal_get_form
 */
function dnchrm_bio_person_manage(array $form = array(), array $form_state = array(), $person = NULL) {
  $scheme = dnchrm_content_type_schema_vars(TRUE, 'persons');
  if (empty($scheme['content_type']) || empty($person)) {
    unset ($scheme);
    return $form;
  }
  if (empty($form_state['storage']['nid'])) {
    $form_state['storage']['nid'] = $person;
  }
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $form_state['storage']['nid'],
  );
  $node = node_load($form_state['storage']['nid']);
  if (empty($node->type) || $node->type != $scheme['content_type']) {
    unset ($node, $scheme);
    drupal_set_message(t('Wrong data !'), 'error', FALSE);
    return $form;
  }
  $data = dncsystem_retrieve_node_data($scheme, $node, NULL, 'default');
  $ntype = node_type_get_type($scheme['content_type']);
  foreach ($scheme['fields'] as $keys => $values) {
    if ($values == 'title') {
      $form[$keys] = array(
        '#type' => 'textfield',
        '#size' => 40,
        '#title' => $ntype->title_label,
        '#required' => TRUE,
        '#weight' => -10,
        '#default_value' => empty($data[$keys]) ? NULL : $data[$keys],
      );
    }
    else {
      $instance = field_info_instance('node', $values, $scheme['content_type']);
      $form[$keys]['#title'] = $instance['label'];
      $form[$keys]['#description'] = $instance['description'];
      $form[$keys]['#required'] = $instance['required'];
      $form[$keys]['#weight'] = $instance['widget']['weight'];
      $form[$keys]['#default_value'] = empty($data[$keys]) ? NULL : $data[$keys];
      $finfo = field_info_field($values);
      if ($instance['widget']['type'] == 'date_popup') {
        $form[$keys]['#type'] = 'date_popup';
        foreach ($instance['widget']['settings'] as $key => $value) {
          if (!empty($value)) {
            $key = '#date_' . preg_replace('/^input_/', '', $key);
            if ($key == '#date_format') {
              $value = dncsystem_granularity_to_date_format($finfo['settings']['granularity']);
            }
            $form[$keys][$key] = $value;
          }
        }
      }
      elseif (preg_match('/^list/', $finfo['type'])) {
        $form[$keys]['#type'] = 'select';
        $form[$keys]['#options'] = $finfo['settings']['allowed_values'];
      }
      elseif ($finfo['type'] == 'entityreference') {
        $form[$keys]['#type'] = 'select';
        $form[$keys]['#options'] = $finfo['settings']['allowed_values'];
      }
      else {
        $form[$keys]['#type'] = 'textfield';
        $form[$keys]['#size'] = empty($instance['widget']['settings']['size']) ? 40 : $instance['widget']['settings']['size'];
        $form[$keys]['#maxlength'] = empty($finfo['settings']['max_length']) ? 255 : $finfo['settings']['max_length'];
      }
      unset ($finfo, $instance);
    }
    if (!user_access('use dnc hrm')) {
      $form[$keys]['#type'] = 'item';
      $form[$keys]['#markup'] = empty($data['v_' . $keys]['#markup']) ? (empty($data[$keys]) ? NULL : $data[$keys]) : (preg_match('/date/', $keys) ? dncsystem_date('l, j F Y', strtotime($data[$keys])) : $data['v_' . $keys]['#markup']);
      $form[$keys]['#prefix'] = '<div class="container-inline">';
      $form[$keys]['#suffix'] = '</div>';
      $form[$keys]['#title'] .= ': ';
      unset ($form[$keys]['#description'], $form[$keys]['#required']);
    }
    else {
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('save'),
        '#weight' => 200,
      );
    }
  }
  return $form;
}

/**
 * Implementing hook_submit for person data manage list
 * 
 * @param array $form
 * @param array $form_state
 */
function dnchrm_bio_person_manage_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  if (user_access('use dnc hrm')) {
    $data = array('nid' => $form_state['values']['nid']);
    $scheme = dnchrm_content_type_schema_vars(TRUE, 'persons');
    foreach ($scheme['fields'] as $keys => $values) {
      if (!empty($form_state['values'][$keys])) {
        $data[$keys] = $form_state['values'][$keys];
      }
    }
    unset ($scheme);
    dncsystem_content_update_data('dnchrm', 'persons', $data);
    unset ($data);
    drupal_set_message(t('Data bio have been updated successfully :)'));
  }
  $form_state['redirect'] = current_path();
}

