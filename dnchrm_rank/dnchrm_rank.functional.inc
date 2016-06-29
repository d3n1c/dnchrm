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
 * Module to handle functional rank of human resource management
 * 
 * Handling functional rank data to handle management of human resources
 */

/**
 * Custom form to manage functional rank of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_rank_functional (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $form_state['storage']['datatitle'] = 'Ranks';
  $form_state['storage']['context'] = 'functional_ranks';
  $form_state['storage']['module'] = 'dnchrm_rank';
  $form_state['storage']['drupalrender'] = 'teaser';
  
  dnchrm_build_tabled_infos_form($form, $form_state);
  if (!empty($form['ctable']['#rows'])) {
    $sorting = array();
    $dump = array();
    foreach ($form['ctable']['#rows'] as $keys => $values) {
      $nid = explode('-', $values[0]['data']['#id']);
      $nid = trim($nid[1]);
      settype($nid, 'int');
      $data = dncsystem_content_get_data('dnchrm_rank', 'functional_ranks', $nid);
      if (!empty($data[$nid])) {
        $data = $data[$nid];
        $tid = $data['type_of_rank'];
        $tdata = dncsystem_content_get_data('dnchrm_rank', 'types_of_rank', $tid);
        if (!empty($tdata[$tid])) {
          $tdata = $tdata[$tid];
          $sorting[$nid] = $tdata['level'];
          $dump[$nid] = $values;
        }
        unset($tid, $tdata);
      }
      unset ($nid, $data);
    }
    $form['ctable']['#rows'] = array();
    arsort($sorting);
    $i = 0;
    foreach ($sorting as $keys => $values) {
      $i++;
      $dump[$keys][0]['data']['#markup'] = $i . '.';
      $form['ctable']['#rows'][] = $dump[$keys];
    }
    unset ($sorting, $dump, $i);
  }
  return $form;
}

/**
 * Custom form to add functional rank
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_rank_functional_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'functional_ranks';
  $form_state['storage']['module'] = 'dnchrm_rank';
  $form_state['storage']['destination'] = 'dnchrm/rank/' . $form_state['storage']['person'] . '/functionals';
  return dnchrm_build_add_item_form($form, $form_state);
}

