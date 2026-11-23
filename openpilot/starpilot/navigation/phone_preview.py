"""Stateless parked preview. Returned coordinates are for display on the companion."""
from openpilot.starpilot.navigation.route_engine import NavigationRoute, distance


def parse_phone_route(payload):
  if not isinstance(payload, dict) or set(payload) != {'version', 'route'} or type(payload['version']) is not int or payload['version'] != 1:
    raise ValueError('Invalid preview version')
  data = payload['route']
  if not isinstance(data, dict) or len(data['legs']) != 1 or not 2 <= len(data['geometry']['coordinates']) <= 1500:
    raise ValueError('Invalid route shape')
  steps = data['legs'][0]['steps']
  if not isinstance(steps, list) or not 1 <= len(steps) <= 256:
    raise ValueError('Invalid steps')
  if any(not isinstance(step, dict) or 'bannerInstructions' in step or not isinstance(step.get('maneuver'), dict) for step in steps):
    raise ValueError('Invalid maneuver')
  return NavigationRoute(data)


def preview_route(payload):
  route = parse_phone_route(payload)
  first = route.progress(route.points[0], None).instruction
  return {'version': 1, 'points': route.preview(), 'steps': len(route.steps), 'firstInstruction': first['text']}

def validate_trip(payload, destination):
  route = parse_phone_route(payload)
  steps = payload['route']['legs'][0]['steps']
  if (steps[0]['maneuver'].get('type') != 'depart' or steps[-1]['maneuver'].get('type') != 'arrive' or
      distance(route.points[-1], (destination['longitude'], destination['latitude'])) > 100 or
      abs(route.steps[-1].end - route.total_distance) > max(10., .02 * route.total_distance) or
      abs(route.cumulative[-1] - route.total_distance) > max(20., .05 * route.total_distance)):
    raise ValueError('Route distances or destination do not match the selected trip')
