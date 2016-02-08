<?php

/**
 * @author Jaap Jansma (CiviCooP) <jaap.jansma@civicoop.org>
 * @license http://www.gnu.org/licenses/agpl-3.0.html
 */
class SegmentSectorCommunity {

    public static function subscribe($contact_id, $segment_id) {
        $uid = self::user_id($contact_id);
        if (!$uid) {
            return;
        }
        $account = user_load($uid);
        $account_edit = array();
        $account_edit["field_pum_user_sector"] = $account->field_pum_user_sector;

        $terms = SegmentTerms::getTermsBySegmentId($segment_id, false);
        foreach($terms as $term) {
            $account_edit["field_pum_user_sector"]["und"] = array(array('tid' => $term->tid));
        }
        user_save($account, $account_edit);
    }

    public static function unsubscribe($contact_id, $segment_id) {
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