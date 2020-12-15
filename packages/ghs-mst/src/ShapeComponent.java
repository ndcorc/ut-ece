/*
 *
 *  Nolan Corcoran
 *  ndc466
 *
 */

import java.awt.*;
import javax.swing.*;

public class ShapeComponent extends JComponent {
    private Shape shape;
    private boolean filled;

  public ShapeComponent(Shape shape) {
    this(shape,false,Color.black);
  }

  public ShapeComponent(Shape shape, boolean filled) {
    this(shape,filled,Color.black);
  }

  public ShapeComponent(Shape shape, Color c) {
    this(shape,false,c);
  }

  public ShapeComponent(Shape shape, boolean filled, Color c) {
    this.shape = shape;
    this.filled = filled;
    setForeground(c);
    updateBounds();
  }

  public Shape getShape() {
    return shape;
  }

  public void setFilled(boolean filled) {
    this.filled = filled;
    repaint();
  }

  synchronized void updateBounds() {
    setBounds(shape.getBounds().x,shape.getBounds().y,
	      shape.getBounds().width+1,shape.getBounds().height+1);
    repaint();
  }

  public Dimension getPreferredSize() {
    return getSize();
  }

  public synchronized void paint(Graphics g) {
    Graphics2D g2 = (Graphics2D) g;
    g2.setColor(getForeground());
    g2.translate(-shape.getBounds().x,-shape.getBounds().y);
    if (filled) {
      g2.fill(shape);
    } else {
      g2.draw(shape);
    }
  }

}



