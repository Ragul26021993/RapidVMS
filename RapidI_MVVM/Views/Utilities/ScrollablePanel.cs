using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Controls.Primitives;

namespace Rapid.Utilities
{
    internal class ScrollablePanel: Panel, IScrollInfo
	{
		#region Properties declarations
        /// <summary>
        /// Dependency property to get and set the ScrollAmount on this panel.
        /// </summary>
        public static readonly DependencyProperty ScrollAmountProperty = DependencyProperty.Register("ScrollAmount", typeof(double), typeof(ScrollablePanel), new PropertyMetadata(40.0D));
        /// <summary>
        /// The no of pixels to be scrolled
        /// </summary>
        public double ScrollAmount
        {
            get { return (double)GetValue(ScrollAmountProperty); }
            set { SetValue(ScrollAmountProperty, value); }
        }
		public static readonly DependencyProperty AnimateDurationProperty = DependencyProperty.Register("AnimateDuration", typeof(int), typeof(ScrollablePanel), new PropertyMetadata(400));
        /// <summary>
        /// The total duration of animation in ms(milli-seconds)
        /// </summary>
        public int AnimateDuration 
		{ 
			get { return (int)GetValue(AnimateDurationProperty); }
            set { SetValue(AnimateDurationProperty, value); }
		}
        #endregion

        #region Panel layout  Implementataion

                public ScrollablePanel()
                {
                    // For use in the IScrollInfo implementation of animation.
                    _trans = new TranslateTransform();
                    this.RenderTransform = _trans;
                }
                protected override Size MeasureOverride(Size constraint)
                {
                    //
                    if (constraint.Width == double.PositiveInfinity || constraint.Height == double.PositiveInfinity)
                        return Size.Empty;
                    //In measure pass updating the scroll bounds<ie Total Extent,Viewport Size, and ScrollOffset.>
                    UpdateScrollBounds(constraint);
                    //The measure logic
                    int childCount = InternalChildren.Count;
                    for (int i = 0; i < childCount; i++)
                        InternalChildren[i].Measure(new Size(constraint.Width, constraint.Height));
                    return constraint;
                }
                protected override Size ArrangeOverride(Size finalSize)
                {
                    try
                    {                    //In arrangement , for being safe , again updating the scroll bounds.
                        UpdateScrollBounds(finalSize);
                        //The arrange logic for the panel.It resembles the logic of Stackpanel in horizontal orientation.
                        int childCount = InternalChildren.Count;
                        for (int i = 0; i < childCount; i++)
                        {
                            UIElement child = InternalChildren[i];
                            double left = 0.0;
                            if (i != 0)
                            {
                                for (int a = 0; a < i; a++)
                                { left += InternalChildren[a].DesiredSize.Width; }
                            }
                            child.Arrange(new Rect(left, 0, child.DesiredSize.Width, finalSize.Height));
                        }
                    }
                    catch (Exception ex)
                    {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ScP05", ex);
            }
                    return finalSize;
                }

        #endregion

        #region IScrollInfo implementation

        private void UpdateScrollBounds(Size availableSize)
        {
            double width = 0;
            bool viewportChanged = false;
            bool extentChanged = false;

            //Calculating the whole largest possible extent width of control.
            foreach (UIElement ele in InternalChildren)
                width += ele.DesiredSize.Width;
            Size extent = new Size(width, availableSize.Height);
            // Update extent
            if (extent != _extent)
            {
                _extent = extent;
                extentChanged = true;
            }
            // Update viewport
            if (availableSize != _viewport)
            {
                _viewport = availableSize;
                viewportChanged = true;
            }
            //Updating The offsests and scroll
            if ((extentChanged || viewportChanged) && _scrollOwner != null)
            {
                _offset.Y = CalculateVerticalOffset(VerticalOffset);
                _offset.X = CalculateHorizontalOffset(HorizontalOffset);
                _scrollOwner.InvalidateScrollInfo();
                Scroll(HorizontalOffset);//, VerticalOffset);
            }
        }

        public ScrollViewer ScrollOwner
        {
            get { return _scrollOwner; }
            set { _scrollOwner = value;
            _scrollOwner.PanningMode = PanningMode.HorizontalOnly;
            }
        }

        public bool CanHorizontallyScroll
        {
            get { return _canHScroll; }
            set { _canHScroll = value; }
        }

        public bool CanVerticallyScroll
        {
            get { return _canVScroll; }
            set { _canVScroll = value; }
        }

        public double HorizontalOffset
        {
            get { return _offset.X; }
        }

        public double VerticalOffset
        {
            get { return _offset.Y; }
        }

        public double ExtentHeight
        {
            get { return _extent.Height; }
        }

        public double ExtentWidth
        {
            get { return _extent.Width; }
        }

        public double ViewportHeight
        {
            get { return _viewport.Height; }
        }

        public double ViewportWidth
        {
            get { return _viewport.Width; }
        }

        public void LineUp()
        {
            SetVerticalOffset(VerticalOffset -ScrollAmount);
        }

        public void LineDown()
        {
            SetVerticalOffset(VerticalOffset +ScrollAmount);
        }

        public void PageUp()
        {
            SetVerticalOffset(VerticalOffset - ViewportHeight);
        }

        public void PageDown()
        {
            SetVerticalOffset(VerticalOffset + ViewportHeight);
        }

        public void MouseWheelUp()
        {
            LineLeft();
        }

        public void MouseWheelDown()
        {
            LineRight();
        }

        public void LineLeft()
        {
            SetHorizontalOffset(HorizontalOffset -ScrollAmount);
        }

        public void LineRight()
        {
            SetHorizontalOffset(HorizontalOffset +ScrollAmount);
        }

        public Rect MakeVisible(Visual visual, Rect rectangle)
        {
            return rectangle;
        }

        public void MouseWheelLeft()
        {
            SetHorizontalOffset(HorizontalOffset -ScrollAmount);
        }

        public void MouseWheelRight()
        {
            SetHorizontalOffset(HorizontalOffset + ScrollAmount);
        }

        public void PageLeft()
        {
            SetHorizontalOffset(HorizontalOffset - ViewportWidth);
        }

        public void PageRight()
        {
            SetHorizontalOffset(HorizontalOffset + ViewportWidth);
        }
        private double oldoffsetval = 0;
        public void SetHorizontalOffset(double offset)
        {
            offset = CalculateHorizontalOffset(offset);
            oldoffsetval = _offset.X;
            _offset.X = offset;

            if (_scrollOwner != null)
                _scrollOwner.InvalidateScrollInfo();

            Scroll(offset);//, VerticalOffset);

            // Force us to realize the correct children
            InvalidateMeasure();
        }

        public void SetVerticalOffset(double offset)
        {
            offset = CalculateVerticalOffset(offset);

            _offset.Y = offset;

            if (_scrollOwner != null)
                _scrollOwner.InvalidateScrollInfo();

            Scroll(HorizontalOffset);//, offset);
        }

        private double CalculateVerticalOffset(double offset)
        {
            if (offset < 0 || _viewport.Height >= _extent.Height)
            {
                offset = 0;
            }
            else
            {
                if (offset + _viewport.Height >= _extent.Height)
                {
                    offset = _extent.Height - _viewport.Height;
                }
            }
            return offset;
        }

        private double CalculateHorizontalOffset(double offset)
        {
            //return offset;
            if (offset < 0 || _viewport.Width >= _extent.Width)
            {
                offset = 0;
            }
            else
            {
                if (offset + _viewport.Width >= _extent.Width)
                {
                    offset = _extent.Width - _viewport.Width;
                }
            }
            return offset;
        }

        private void Scroll(double xOffset)//, double yOffset)
        {
            int TmpDuration;

            //if (Math.Abs(xOffset - oldoffsetval) <= (double)GetValue(ScrollAmountProperty))
                //TmpDuration = (int)(Math.Abs(xOffset - oldoffsetval) / (double)GetValue(ScrollAmountProperty) * AnimateDuration);
            //else
                TmpDuration = AnimateDuration;
            DoubleAnimation anim = new DoubleAnimation(-xOffset, new Duration(TimeSpan.FromMilliseconds(TmpDuration)));
            PropertyPath p = new PropertyPath("(0).(1)", RenderTransformProperty, TranslateTransform.XProperty);
            Storyboard.SetTargetProperty(anim, p);

            Storyboard sb = new Storyboard();
            sb.Children.Add(anim);
            EventHandler handler = null;
            handler = delegate
            {
                sb.Completed -= handler;
                sb.Remove(this);
                _trans.X = -xOffset;
            };
            sb.Completed += handler;
            sb.Begin(this, true);
        }

        private TranslateTransform _trans = new TranslateTransform();
        private ScrollViewer _scrollOwner;
        private bool _canHScroll = false;
        private bool _canVScroll = false;
        private Size _extent = new Size(0, 0);
        private Size _viewport = new Size(0, 0);
        protected internal Point _offset;

        #endregion
		}
}