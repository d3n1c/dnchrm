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
 * Module to handle formal addreses of human resource management
 * 
 * Handling formal addreses data to handle management of human resources
 */

/**
 * Custom form to manage formal addresses of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_dynamic_address(array $form = array(), array $form_state = array(), $person = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['formal_addresses'])) {
    unset ($activecontext);
    return;
  }
  unset ($activecontext);
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $form_state['storage']['datatitle'] = 'Formal Address';
  $form_state['storage']['context'] = 'formal_addresses';
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['wrapfields'] = TRUE;
  $options = dnchrm_get_data_by_person($person, 'dnchrm_dynamic', 'social_identities', NULL, 'full');
  if (!empty($options)) {
    foreach ($options as $values) {
      $form_state['storage']['entityreference_custom']['social_identity']['options'][$values['nid']] = '(' . $values['v_type_of_identity']['#markup'] . ') ' . $values['title'];
    }
  }
  unset($options);
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Custom form to add formal addresses
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_dynamic_address_add(array $form = array(), array $form_state = array(), $person = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['formal_addresses'])) {
    unset ($activecontext);
    return;
  }
  unset ($activecontext);
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'formal_addresses';
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['destination'] = 'dnchrm/bio/' . $form_state['storage']['person'] . '/addresses';
  return dnchrm_build_add_item_form($form, $form_state);
}

