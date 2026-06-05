/**
 * קובץ רכיב מותאם עבור A-Frame - Pinch Detection Driver
 * מקשיב לחיישני מעקב הידיים של ה-Quest ומייצר אירוע נגיעה/צביטה
 */
AFRAME.registerComponent('pinchable', {
  schema: {
    pinchDistance: { default: 0.05 } // מרחק רגישות הצביטה בין האגודל לאצבע המורה
  },

  init: function () {
    // רשימת כפות הידיים הזמינות בסצנה
    this.handEls = document.querySelectorAll('[hand-tracking-controls]');
    this.bindMethods();
  },

  bindMethods: function () {
    this.checkPinch = this.checkPinch.bind(this);
  },

  tick: function () {
    // לולאה שרצה בכל פריים של המשקפיים ובודקת האם המשתמש מבצע צביטה על האובייקט
    for (let i = 0; i < this.handEls.length; i++) {
      this.checkPinch(this.handEls[i]);
    }
  },

  checkPinch: function (handEl) {
    // ודא שהחיישנים של היד פעילים ונראים לעין
    if (!handEl.components['hand-tracking-controls'] || !handEl.visible) return;

    // שליפת הנתונים הגולמיים של עצמות היד (האגודל והאצבע המורה)
    const handComponents = handEl.components['hand-tracking-controls'];
    const indexTip = handComponents.indexTip;
    const thumbTip = handComponents.thumbTip;

    // אם המשקפיים לא מזהים את האצבעות ברגע זה - עצור
    if (!indexTip || !thumbTip) return;

    // מיקום מרכז האובייקט (הבלון) במרחב
    const elPos = new THREE.Vector3();
    this.el.object3D.getWorldPosition(elPos);

    // חישוב המרחק בין קצה האצבע המורה לבלון
    const distanceToIndex = indexTip.position.distanceTo(elPos);

    // אם האצבע המורה נמצאת פיזית בתוך רדיוס הבלון (פלוס טווח ביטחון קטן)
    if (distanceToIndex < (this.el.components.geometry.data.radius + 0.08)) {
      
      // חישוב המרחק הפנימי בין האגודל לאצבע המורה (בדיקת ג'סטת צביטה - Pinch)
      const pinchDistance = indexTip.position.distanceTo(thumbTip.position);

      // אם המרחק קטן מ-5 ס"מ (כלומר המשתמש סגר את האצבעות לצביטה באוויר בתוך הבלון)
      if (pinchDistance < this.data.pinchDistance) {
        // תירה אירוע חומרתי קסטום בשם 'pinched'
        this.el.emit('pinched', { hand: handEl });
      }
    }
  }
});
