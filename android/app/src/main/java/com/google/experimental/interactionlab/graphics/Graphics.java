package com.google.experimental.interactionlab.graphics;

import processing.core.PApplet;

public class Graphics {

    public static enum GLYPH_STYLE {DOT, LINE, ARROW};

    public static float[] getIntersection(float x1, float y1, float x2, float y2, float angle) {
        float yaw = (float) (angle * Math.PI / 180);

        //center
        float xc = x1 + (x2 - x1) / 2;
        float yc = y1 + (y2 - y1) / 2;

        //point towards the angle
        float xe = (float) Math.cos(yaw);
        float ye = (float) Math.sin(yaw);


        float y = (float) (Math.tan(yaw) * (x2 - xc));
        boolean intersectY = (yc + y > y1) && (yc + y < y2);
        boolean intersectRight = intersectY && xe > 0;
        boolean intersectLeft = intersectY && xe <= 0;
        if (intersectLeft)
            y = -y;

        float x = (float) (Math.tan(yaw + Math.PI / 2) * (y2 - y1) / 2);
        boolean intersectX = (xc + x > x1) && (xc + x < x2);
        boolean intersectTop = intersectX && ye <= 0;
        boolean intersectBottom = intersectX && ye >= 0;
        if (intersectBottom)
            x = -x;

        if (intersectLeft || intersectRight)
            return new float[]{intersectLeft ? x1 : x2, yc + y};
        else if (intersectTop || intersectBottom)
            return new float[]{xc + x, intersectTop ? y1 : y2};
        else
            return null;
    }

    public static void drawRectangleHighlightLine(PApplet p, float x1, float y1, float x2, float y2, float angle, float w, int colorStart, int colorEnd) {
        float[] a = getIntersection(x1, y1, x2, y2, angle - w / 2);
        if (a == null)
            return;

        float[] c = getIntersection(x1, y1, x2, y2, angle + w / 2);
        if (c == null)
            return;

        float[] b = null;

        //if points share neither x nor y, then we need to add a point to a line segment
        if ((a[0] != c[0]) && (a[1] != c[1])) {
            b = new float[]{0, 0};

            //determine where to insert the extra point
            b[1] = (c[0] > a[0]) ? y1 : y2;
            b[0] = (c[1] > a[1]) ? x2 : x1;

        }

    /*
    ellipse(a[0], a[1], 50, 50);
    if (b != null)
      ellipse(b[0], b[1], 20, 20);
    ellipse(c[0], c[1], 100, 100);
    */

        p.beginShape(PApplet.LINES);

        p.stroke(colorStart);
        p.vertex(a[0], a[1]);

        if (b != null) {
            p.vertex(b[0], b[1]);
            p.vertex(b[0], b[1]);
        }

        p.stroke(colorEnd);
        p.vertex(c[0], c[1]);
        p.endShape();

    }

    public static void drawEdgeGlyph(PApplet p, float angle) {
        drawEdgeGlyph(p, angle, p.width/16);
    }

    public static void drawEdgeGlyph(PApplet p, float angle, float cr) {

        float x1 = p.width / 2;
        float y1 = p.height / 2;
        //point in the direction of the sound (radius: height)
        float x2 = -p.height * p.cos((float) (angle * Math.PI / 180));
        float y2 = -p.height * p.sin((float) (angle * Math.PI / 180));

        //calculate where the (x1, y1, x2, y2) line intersects the screen
        float yRight = (float) (Math.tan(angle * Math.PI / 180) * x1);
        float yLeft = -yRight;
        float xTop = -y1 / (float) (Math.tan(angle * Math.PI / 180));
        float xBottom = -xTop;


        // use angle to index the hue (0-360)
        p.fill(angle, 40, 100, 200);
        p.noStroke();

        if (angle >= 90 && angle < 270) //right side
            p.ellipse(p.width, y1 + yRight, cr, cr);
        else //left side
            p.ellipse(0, y1 + yLeft, cr, cr);

        if (angle >= 0 && angle < 180) //top
            p.ellipse(x1 + xTop, 0, cr, cr);
        else //bottom
            p.ellipse(x1 + xBottom, p.height, cr, cr);
    }

    // Draw map for how color is mapped to angle
    public static void drawCircleMap(PApplet p, float angle, boolean showLeft, boolean showRight, boolean showTop, boolean showBottom) {
        //hue = 0-360, saturation = 0-100, brightness = 0-100
        p.colorMode(p.HSB, 360, 100, 100);

        float angle_rad = angle * p.PI / 180;
        float ra = p.width / 8;
        float cx = p.width - p.width / 8;
        float cy = p.width / 8;
        p.noFill();

        ra /= 2;

        for (float i = 0; i < 360; i += 0.1) {
            if (!isIncluded(i, showLeft, showRight, showTop, showBottom))
                continue;

            float rad = i * p.PI / 180;
            p.stroke(i, 40, 100);
            p.line(cx, cy, cx - ra * p.cos(rad), cy - ra * p.sin(rad));
        }

        p.fill(angle, 40, 100);
        p.noStroke();
        p.ellipse(cx - ra * 1.3f * p.cos(angle_rad), cy - ra * 1.3f * p.sin(angle_rad), ra / 2, ra / 2);

    }

    public static boolean isIncluded(float angle, boolean showLeft, boolean showRight, boolean showTop, boolean showBottom) {
        return !((((angle >= 315 && angle < 360) || (angle >= 0 && angle < 45)) && !showLeft) ||
                (angle >= 45 && angle < 135 && !showTop) ||
                (angle >= 135 && angle < 225 && !showRight) ||
                (angle >= 225 && angle < 315 && !showBottom));

    }

    public static void drawGlyph(PApplet p, float angle, float tdy, GLYPH_STYLE style) {
        float rr = tdy / 2;
        float angle_rad = angle * p.PI / 180;
        float cx = -tdy / 2;
        float cy = -tdy / 4;
        p.noFill();

        if (style == GLYPH_STYLE.LINE || style == GLYPH_STYLE.DOT) {
           p.ellipse(cx, cy, rr, rr);
        }

        rr /= 2;

        rr /= 1.2;
        p.fill(angle, 40, 100);
        p.stroke(angle, 40, 100);
        //ellipse(cx - rr * cos(angle_rad), cy - rr * sin(angle_rad), rr/2, rr/2);

        //line(cx, cy, cx - rr * cos(angle_rad), cy - rr * sin(angle_rad));

        if (style == GLYPH_STYLE.ARROW) {
            p.line(cx + rr * p.cos(angle_rad), cy + rr * p.sin(angle_rad), cx - rr * p.cos(angle_rad), cy - rr * p.sin(angle_rad));
            p.line(cx - rr * p.cos(angle_rad + p.PI / 2), cy - rr * p.sin(angle_rad + p.PI / 2), cx - rr * p.cos(angle_rad), cy - rr * p.sin(angle_rad));
            p.line(cx - rr * p.cos(angle_rad - p.PI / 2), cy - rr * p.sin(angle_rad - p.PI / 2), cx - rr * p.cos(angle_rad), cy - rr * p.sin(angle_rad));
        }
        else if (style == GLYPH_STYLE.LINE) {
            p.line(cx, cy, cx - rr * p.cos(angle_rad), cy - rr * p.sin(angle_rad));
        }
        else if (style == GLYPH_STYLE.DOT) {
            p.ellipse(cx - rr * p.cos(angle_rad), cy - rr * p.sin(angle_rad), rr/2, rr/2);
        }


    }
}
