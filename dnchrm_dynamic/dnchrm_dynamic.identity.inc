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
 * Module to handle social identities of human resource management
 * 
 * Handling social identities data to handle management of human resources
 */

/**
 * Custom form to manage social identities of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_dynamic_identity (array $form = array(), array $form_state = array(), $person = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['social_identities'])) {
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
  $form_state['storage']['datatitle'] = 'Social Identities';
  $form_state['storage']['context'] = 'social_identities';
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['getnewref'] = array(
    'path' => 'dnchrm-dynamic/nojs/form/setidentity',
    'title' => 'Types of Identity',
    'size' => 'medium',
  );
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Produce popup form to add new personal characteristic type
 * 
 * @param string $js
 */
function dnchrm_dynamic_add_new_types_of_identity_popup($js = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['social_identities'])) {
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
  $form_state['title'] = t('Types of Identity');
  $form_state['ajax'] = TRUE;
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['context'] = 'types_of_identity';
  $output = ctools_modal_form_wrapper('dnchrm_register_reference', $form_state);
  if (!empty($form_state['executed'])) {
    $output = array();
    $output[] = ctools_modal_command_dismiss(t('Success!'));
    $output[] = ctools_ajax_command_reload();
  }
  print ajax_render($output);
}

/**
 * Custom form to add social identities
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_dynamic_identity_add(array $form = array(), array $form_state = array(), $person = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['social_identities'])) {
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
  
  $form_state['storage']['context'] = 'social_identities';
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['destination'] = 'dnchrm/bio/' . $form_state['storage']['person'] . '/identities';
  return dnchrm_build_add_item_form($form, $form_state);
}

