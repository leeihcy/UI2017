task:

   
2. 20121211_24:42 如果有一个对象配置在XML中，但它的实现是在另一个DLL中，并非一个UI内部控件
   那么这个控件要怎么在UIBUILDER中展现，或者编辑属性？
     
3. 20121212 9:28  怎么对属性按钮类型排序显示？
   
--------------------------------------------------------------------------------------------------------------------------   
5. 20121212 17:45 现有的资源无法实现属性编辑功能，先暂停UIBuilder的开发，继续丰富UI控件库，至少要增加TreeView和ListView   
   2013.6.18 9:08 ListCtrl的重构基本完成，重新开始UIEditor的开发
   
   
4. 20121212 17:44 如果我要有UIBuilder工程中，自己使用UI控件，那么就会有两个UIApplication变量，但现在的机制根本不支持。
   考虑下CreateUIApplicationInstance，然后废除所有的UI_XXX 全局函数调用，都改成 IUIApplication->形式。
   问题是我一个窗口对象怎么知道我在哪个 application 中？
   已完成了，然后每个Object都可以获取到自己属于哪个app   
  
  
1. 将UIDLL中的 uiapplication里的各种resmanager替换成skinmanager
   不过该任务要在uibuilder中的skinmanager丰富完之后再做  