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

