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
 * Module to handle mastery of languages of human resource management
 * 
 * Handling mastery of languages data to handle management of human resources
 */

/**
 * Custom form to manage mastery of languages of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_dynamic_language (array $form = array(), array $form_state = array(), $person = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['mastery_of_languages'])) {
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
  $form_state['storage']['datatitle'] = 'Mastery of Languages';
  $form_state['storage']['context'] = 'mastery_of_languages';
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['wrapfields'] = TRUE;
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Custom form to add mastery of languages
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_dynamic_language_add(array $form = array(), array $form_state = array(), $person = NULL) {
  $activecontext = variable_get('dnchrm_dynamic_active_context', array());
  if (empty($activecontext['mastery_of_languages'])) {
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
  
  $form_state['storage']['context'] = 'mastery_of_languages';
  $form_state['storage']['module'] = 'dnchrm_dynamic';
  $form_state['storage']['destination'] = 'dnchrm/bio/' . $form_state['storage']['person'] . '/languages';
  return dnchrm_build_add_item_form($form, $form_state);
}

