/**
 * קובץ רכיב מותאם עבור A-Frame - Pure Touch Detection Driver
 * מבוסס על הדוגמה הרשמית שעבדה - מקשיב לנגיעה פיזית של האצבע
 */
AFRAME.registerComponent('pressable', {
  init: function () {
    // מציאת כפות הידיים בסצנה
    this.handEls = document.querySelectorAll('[hand-tracking-controls]');
    this.pressed = false;
    this.bindMethods();
  },

  bindMethods: function () {
    this.checkPress = this.checkPress.bind(this);
  },

  tick: function () {
    // לולאה שרצה בכל פריים ובודקת האם אחת האצבעות נוגעת פיזית באובייקט
    for (let i = 0; i < this.handEls.length; i++) {
      this.checkPress(this.handEls[i]);
    }
  },

  checkPress: function (handEl) {
    // ודא שחיישני היד נראים ופעילים
    if (!handEl.components['hand-tracking-controls'] || !handEl.visible) return;

    const handComponents = handEl.components['hand-tracking-controls'];
    const indexTip = handComponents.indexTip; // קצה האצבע המורה

    // אם המשקפיים לא מזהים את קצה האצבע כרגע - עצור
    if (!indexTip) return;

    // שליפת המיקום המדויק של הבלון במרחב
    const elPos = new THREE.Vector3();
    this.el.object3D.getWorldPosition(elPos);

    // חישוב המרחק בין קצה האצבע המורה של היד לבין מרכז הבלון
    const distanceToIndex = indexTip.position.distanceTo(elPos);

    // שליפת הרדיוס של הבלון (בקוד שלנו זה 0.12 מטר)
    const radius = this.el.components.geometry.data.radius;

    // אם האצבע נכנסה פיזית אל תוך גבולות הבלון (המרחק קטן מהרדיוס + טווח ביטחון קטן של נגיעה)
    if (distanceToIndex < (radius + 0.05)) {
      if (!this.pressed) {
        this.pressed = true;
        // יריקת אירוע חומרתי בשם 'pressed' שמפעיל את הפיצוץ
        this.el.emit('pressed', { hand: handEl });
      }
    } else {
      // ברגע שהיד יוצאת מהבלון, מאפסים את היכולת ללחוץ שוב
      this.pressed = false;
    }
  }
});