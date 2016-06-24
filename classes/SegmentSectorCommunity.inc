<?php

/**
 * @author Jaap Jansma (CiviCooP) <jaap.jansma@civicoop.org>
 * @license http://www.gnu.org/licenses/agpl-3.0.html
 */
class SegmentSectorCommunity {

    private static $is_running = false;

    public static function subscribe($contact_id, $segment_id, $edit=array()) {
        self::$is_running = true;
        $uid = self::user_id($contact_id);
        if (!$uid) {
            return;
        }
        $account = user_load($uid);
        $account_edit = $edit;
        $account_edit["field_pum_user_sector"] = $account->field_pum_user_sector;

        $terms = SegmentTerms::getTermsBySegmentId($segment_id, false);
        foreach($terms as $term) {
            $account_edit["field_pum_user_sector"]["und"] = array(array('tid' => $term->tid));
        }
        user_save($account, $account_edit);
    }

    public static function unsubscribe($contact_id, $segment_id) {
        self::$is_running = true;
        $uid = self::user_id($contact_id);
        if (!$uid) {
            return;
        }
        $account = user_load($uid);
        $account_edit = array();
        $account_edit["field_pum_user_sector"] = $account->field_pum_user_sector;

        if (isset($account_edit["field_pum_user_sector"]['und']) && is_array($account_edit["field_pum_user_sector"]['und'])) {
            $terms = SegmentTerms::getTermsBySegmentId($segment_id, false);
            foreach ($account_edit["field_pum_user_sector"]['und'] as $v => $t) {
                foreach($terms as $term) {
                    if ($term->tid == $t['tid']) {
                        unset($account_edit["field_pum_user_sector"]['und'][$v]);
                    }
                }
            }
        }
        user_save($account, $account_edit);
    }

    public static function subscribe_main_sector($user_id, &$edit) {
        $contact_id = self::contact_id($user_id);
        if (!$contact_id) {
            return;
        }

        $main_segment_id = false;
        try {
            $main_segment_id = civicrm_api3('ContactSegment', 'getvalue', array('contact_id' => $contact_id, 'is_main' => '1', 'return' => 'segment_id'));
            $terms = SegmentTerms::getTermsBySegmentId($main_segment_id, false);
            foreach($terms as $term) {
                $edit["field_pum_user_sector"]["und"] = array(array('tid' => $term->tid));
            }
        } catch (Exception $e) {
            // Do nothing.
        }
        if (!$main_segment_id) {
            return;
        }
    }

    protected static function contact_id($user_id) {
        try {
            return civicrm_api3('UFMatch', 'getvalue', array(
              'uf_id' => $user_id,
              'return' => 'contact_id',
            ));
        } catch (Exception $e) {
            //do nothing
        }
        return false;
    }

    protected static function user_id($contact_id) {
        try {
            return civicrm_api3('UFMatch', 'getvalue', array(
                'contact_id' => $contact_id,
                'return' => 'uf_id',
            ));
        } catch (Exception $e) {
            //do nothing
        }
        return false;
    }

}