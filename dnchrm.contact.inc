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
 * Handling all operation to manipulate contact data
 */

/**
 * Custom form to manage contact of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_bio_contact_manage (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($person)) {
    return $form;
  }
  
  $form_state['storage']['datatitle'] = 'Personal Datas';
  $form_state['storage']['person'] = $person;
  $form_state['storage']['context'] = 'contacts';
  $form_state['storage']['getnewref'] = array(
    'path' => 'dnchrm-bio/nojs/form/settcontact',
    'title' => 'Type of Contact',
  );
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Implementing hook_submit for contact manage form
 * 
 * @param array $form
 * @param array $form_state
 */
function dnchrm_bio_contact_manage_submit(&$form, &$form_state) {
  form_state_values_clean($form_state);
  $form_state['redirect'] = current_path();
  if (!user_access('use dnc hrm')) {
    drupal_set_message(t('Sorry you\'re not authorized :('), 'error', FALSE);
    return;
  }
  if (empty($form_state['input']['items']) || empty($form_state['values']['person'])) {
    return;
  }
  $scheme = dnchrm_content_type_schema_vars(TRUE, 'contacts');
  foreach ($form_state['input']['items'] as $keys => $values) {
    if (!empty($values['delete'])) {
      dncsystem_content_delete('dnchrm', 'contacts', $keys);
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
      $check = dncsystem_content_get_data('dnchrm', 'types_of_contact', $data['type_of_contact'], 'title', 'default');
      if (!empty($check[$data['type_of_contact']]['title'])) {
        $check = $check[$data['type_of_contact']]['title'];
        if (preg_match('/mail/', $check)) {
          if (!valid_email_address($data['title'])) {
            $error = TRUE;
          }
        }
      }
      unset($check);
      if (empty($error)) {
        dncsystem_content_update_data('dnchrm', 'contacts', $data);
      }
      else {
        drupal_set_message(t('Invalid email address :('), 'error', FALSE);
        unset($error);
      }
      unset ($data);
    }
  }
  unset ($scheme);
  drupal_set_message(t('Update contacts have been done successfully :)'));
}

/**
 * Custom form to add contact
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_bio_contact_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'contacts';
  return dnchrm_build_add_item_form($form, $form_state);
}

/**
 * Produce popup form to add new types of contact
 * 
 * @param string $js
 */
function dnchrm_add_new_tcontact_popup($js = NULL) {
  // degrade half-assed to no js
  if(!$js) {
    $contexts = array(
      'context' => 'types_of_contact',
    );
    $contexts = urlencode(base64_encode(serialize($contexts)));
    return drupal_get_form('dnchrm_register_reference', $contexts);
  }
  
  // adelante!
  ctools_include('modal');
  ctools_include('ajax');
  $form_state['title'] = t('Register Type of Contact');
  $form_state['ajax'] = TRUE;
  $form_state['storage']['context'] = 'types_of_contact';
  $output = ctools_modal_form_wrapper('dnchrm_register_reference', $form_state);
  if (!empty($form_state['executed'])) {
    $output = array();
    $output[] = ctools_modal_command_dismiss(t('Success!'));
    $output[] = ctools_ajax_command_reload();
  }
  print ajax_render($output);
}

