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
 * Handling all operation to manipulate register data
 */

/**
 * Generating form api to listing and update person's registers
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * 
 * @return array
 *   form array to render by drupal_get_form
 */
function dnchrm_bio_register_manage (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($person)) {
    return $form;
  }
  $form_state['storage']['datatitle'] = 'Personal Datas';
  $form_state['storage']['person'] = $person;
  $form_state['storage']['context'] = 'registers';
  $form_state['storage']['drupalrender'] = 'teaser';
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Implementing hook_submit for register managing form
 * 
 * @param array $form
 * @param array $form_state
 * 
 * @return type
 *   redirecting and set message
 */
function dnchrm_bio_register_manage_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  if (!user_access('use dnc hrm')) {
    drupal_set_message(t('Sorry you\'re not authorized :('), 'error', FALSE);
    return;
  }
  if (empty($form_state['input']['items']) || empty($form_state['values']['person'])) {
    return;
  }
  $scheme = dnchrm_content_type_schema_vars(TRUE, 'registers');
  foreach ($form_state['input']['items'] as $keys => $values) {
    if (!empty($values['delete'])) {
      dncsystem_content_delete('dnchrm', 'registers', $keys);
    }
  }
  unset ($scheme);
  drupal_set_message(t('Update registers have been done successfully :)'));
}

/**
 * Generate form api to add person's register
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_bio_register_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'registers';
  $form_state['storage']['checkedfields'] = array('effective_date', 'legal_basis');
  return dnchrm_build_add_item_form($form, $form_state);
}

/**
 * Getting person's register ID
 * 
 * @param int $person
 *   NID of the person
 * @param boolean $all
 *   TRUE will return all data, FALSE will return the new one only
 *   default to FALSE
 * 
 * @return string|array
 *   return string if found only one info, array if more, and NULL if
 *   nothing data found
 */
function dnchrm_register_get_by_person($person, $all = FALSE) {
  $scheme = dnchrm_content_type_schema_vars(TRUE, 'registers');
  if (empty($scheme['content_type'])) {
    unset($scheme);
    return;
  }
  
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node');
  $query->entityCondition('bundle', $scheme['content_type']);
  $query->propertyCondition('status', NODE_PUBLISHED);
  $query->addMetaData('account', user_load(1));
  foreach ($scheme['fields'] as $keys => $values) {
    if (in_array($keys, array('person', 'effective_date'))) {
      if ($values == 'title') {
        if ($keys == 'person') {
          $query->propertyCondition('title', $person);
        }
        else {
          $query->propertyOrderBy('title', 'DESC');
        }
      }
      else {
        $finfo = field_info_field($values);
        $column = isset($finfo['columns']['target_id']) ? 'target_id' : 'value';
        if ($keys == 'person') {
          $query->fieldCondition($values, $column, $person);
        }
        else {
          $query->fieldOrderBy($values, $column, 'DESC');
        }
        unset ($column, $finfo);
      }
    }
  }
  if (empty($all)) {
    $query->range(0, 1);
  }
  $result = $query->execute();
  unset ($query);
  if (empty($result['node'])) {
    unset ($result, $scheme);
    return;
  }
  $nodes = node_load_multiple(array_keys($result['node']));
  unset ($result);
  $return = array();
  foreach ($nodes as $node) {
    if (empty($all)) {
      $return = dncsystem_retrieve_node_data($scheme, $node, 'title');
    }
    else {
      $return[$node->nid] = dncsystem_retrieve_node_data($scheme, $node, 'title');
    }
  }
  unset ($scheme, $node, $nodes);
  return $return;
}

