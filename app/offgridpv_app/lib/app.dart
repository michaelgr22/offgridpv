import 'package:flutter/material.dart';
import 'package:offgridpv_app/presentation/pages/home_page.dart';

class App extends StatelessWidget {
  static const String _title = 'offgridpv';

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: _title,
      theme: ThemeData(
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: HomePage(),
    );
  }
}
