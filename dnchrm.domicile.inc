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
 * Handling all operation to manipulate domicile data
 */

/**
 * Generating form api to listing and update person's domiciles
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * 
 * @return array
 *   form array to render by drupal_get_form
 */
function dnchrm_bio_domicile_manage (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($person)) {
    return $form;
  }
  $form_state['storage']['datatitle'] = 'Personal Datas';
  $form_state['storage']['person'] = $person;
  $form_state['storage']['context'] = 'domiciles';
  $form_state['storage']['wrapfields'] = TRUE;
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Implementing hook_submit for domicile managing form
 * 
 * @param array $form
 * @param array $form_state
 * 
 * @return type
 *   redirecting and set message
 */
function dnchrm_bio_domicile_manage_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  if (!user_access('use dnc hrm')) {
    drupal_set_message(t('Sorry you\'re not authorized :('), 'error', FALSE);
    return;
  }
  if (empty($form_state['input']['items']) || empty($form_state['values']['person'])) {
    return;
  }
  $scheme = dnchrm_content_type_schema_vars(TRUE, 'domiciles');
  foreach ($form_state['input']['items'] as $keys => $values) {
    if (!empty($values['delete'])) {
      dncsystem_content_delete('dnchrm', 'domiciles', $keys);
    }
    else {
      $data = array(
        'nid' => $keys,
      );
      foreach ($scheme['fields'] as $key => $value) {
        $fval = $key == 'person' ? $form_state['values']['person'] : (empty($values[$key]) ? NULL : $values[$key]);
        $data[$key] = $fval;
        unset ($fval);
      }
      dncsystem_content_update_data('dnchrm', 'domiciles', $data);
      unset ($data);
    }
  }
  unset ($scheme);
  drupal_set_message(t('Update domiciles have been done successfully :)'));
}

/**
 * Generate form api to add person's domicile
 * 
 * @param array $form
 * @param array $form_state
 * @param integer $person
 *   NID of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_bio_domicile_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $form_state['storage']['context'] = 'domiciles';
  $form_state['storage']['checkedfields'] = array('city', 'province');
  return dnchrm_build_add_item_form($form, $form_state);
}

