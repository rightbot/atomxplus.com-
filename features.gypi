{
  # If it looks stupid but it works it ain't stupid.
  'variables': {
    'variables': {
      'enable_osr%': 1,  # FIXME(alexeykuzmin)
      'enable_pdf_viewer%': 0,  # FIXME(deepak1556)
      'enable_run_as_node%': 1,
      'enable_view_api%': 0,
    },
    'enable_osr%': '<(enable_osr)',
    'enable_pdf_viewer%': '<(enable_pdf_viewer)',
    'enable_run_as_node%': '<(enable_run_as_node)',
    'enable_view_api%': '<(enable_view_api)',
  },
}
